// zink_jni.cpp
// JNI entry points exposed to Kotlin.
//
// Threading model:
//   - glk_main() (bocfel's interpreter loop) runs on a dedicated C++ thread.
//   - Output: bocfel calls g_put_char -> chars accumulate in g_output_buf.
//             Kotlin calls zinkPollOutput() to drain the buffer periodically.
//   - Input:  Kotlin calls zinkInput(line) to unblock bocfel's g_get_line wait.

#include <jni.h>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <android/log.h>

#include "bocfel/zterp.h"

#define LOG_TAG "ZinkJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ---------------------------------------------------------------------------
// Forward declarations from zink_glk.cpp
// ---------------------------------------------------------------------------
extern void zink_set_callbacks(std::function<void(uint32_t)> put_char,
                               std::function<std::string()>  get_line);

// Bocfel's Glk entry point
extern "C" void glk_main();

// ---------------------------------------------------------------------------
// Shared state between interpreter thread and Kotlin
// ---------------------------------------------------------------------------

// Output buffer — bocfel writes, Kotlin reads
static std::mutex              g_out_mutex;
static std::string             g_output_buf;

// Input queue — Kotlin writes one line, bocfel blocks until it arrives
static std::mutex              g_in_mutex;
static std::condition_variable g_in_cv;
static std::string             g_input_line;
static bool                    g_input_ready = false;

// Interpreter thread lifecycle
static std::thread             g_interp_thread;
static std::atomic<bool>       g_running{false};

// ---------------------------------------------------------------------------
// Callback implementations — called from bocfel's Glk layer
// ---------------------------------------------------------------------------

static void on_put_char(uint32_t codepoint) {
    // Encode Unicode codepoint as UTF-8 and append to output buffer.
    std::lock_guard<std::mutex> lock(g_out_mutex);
    if (codepoint < 0x80) {
        g_output_buf += static_cast<char>(codepoint);
    } else if (codepoint < 0x800) {
        g_output_buf += static_cast<char>(0xC0 | (codepoint >> 6));
        g_output_buf += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint < 0x10000) {
        g_output_buf += static_cast<char>(0xE0 | (codepoint >> 12));
        g_output_buf += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        g_output_buf += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else {
        g_output_buf += static_cast<char>(0xF0 | (codepoint >> 18));
        g_output_buf += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        g_output_buf += static_cast<char>(0x80 | ((codepoint >> 6)  & 0x3F));
        g_output_buf += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
}

static std::string on_get_line() {
    // Block until Kotlin calls zinkInput() with a line.
    std::unique_lock<std::mutex> lock(g_in_mutex);
    g_in_cv.wait(lock, [] { return g_input_ready || !g_running; });
    std::string line = g_input_line;
    g_input_line.clear();
    g_input_ready = false;
    return line;
}

// ---------------------------------------------------------------------------
// Interpreter thread entry
// ---------------------------------------------------------------------------

static void interpreter_thread(std::string story_path) {
    LOGI("Interpreter thread started: %s", story_path.c_str());

    // Point bocfel at our story file via the glkunix argv mechanism.
    // glkunix_startup_code in glkstart.cpp reads these.
    static char arg0[] = "zink";
    static char arg1[1024];
    std::snprintf(arg1, sizeof(arg1), "%s", story_path.c_str());
    static char* argv[] = { arg0, arg1, nullptr };

    // Bocfel's glkstart.cpp startup reads glkunix_arguments via
    // glkunix_startup_code, which is called by the Glk library startup.
    // Since we're our own Glk, we call it directly here.
    extern int glkunix_startup_code(void*);
    struct { int argc; char** argv; } startup_data = { 2, argv };
    if (!glkunix_startup_code(&startup_data)) {
        LOGE("glkunix_startup_code failed");
        g_running = false;
        return;
    }

    try {
        glk_main();
    } catch (const std::exception& e) {
        LOGI("glk_main exited: %s", e.what());
    } catch (...) {
        LOGI("glk_main exited");
    }

    g_running = false;
    // Unblock any waiting on_get_line
    g_in_cv.notify_all();
    LOGI("Interpreter thread finished");
}

// ---------------------------------------------------------------------------
// JNI exports
// ---------------------------------------------------------------------------

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_zink_kompakt_engine_ZinkEngine_version(JNIEnv *env, jobject) {
    return env->NewStringUTF(ZTERP_VERSION);
}

JNIEXPORT jboolean JNICALL
Java_com_zink_kompakt_engine_ZinkEngine_start(JNIEnv *env, jobject, jstring path) {
    if (g_running) {
        LOGE("start() called while interpreter already running");
        return JNI_FALSE;
    }

    const char* path_cstr = env->GetStringUTFChars(path, nullptr);
    std::string story_path(path_cstr);
    env->ReleaseStringUTFChars(path, path_cstr);

    // Reset shared state
    {
        std::lock_guard<std::mutex> lock(g_out_mutex);
        g_output_buf.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_in_mutex);
        g_input_line.clear();
        g_input_ready = false;
    }

    // Wire up Glk callbacks
    zink_set_callbacks(on_put_char, on_get_line);

    g_running = true;
    g_interp_thread = std::thread(interpreter_thread, story_path);
    g_interp_thread.detach();

    LOGI("Interpreter started for: %s", story_path.c_str());
    return JNI_TRUE;
}

JNIEXPORT void JNICALL
Java_com_zink_kompakt_engine_ZinkEngine_input(JNIEnv *env, jobject, jstring line) {
    const char* line_cstr = env->GetStringUTFChars(line, nullptr);
    {
        std::lock_guard<std::mutex> lock(g_in_mutex);
        g_input_line = line_cstr;
        g_input_ready = true;
    }
    env->ReleaseStringUTFChars(line, line_cstr);
    g_in_cv.notify_one();
}

JNIEXPORT jstring JNICALL
Java_com_zink_kompakt_engine_ZinkEngine_pollOutput(JNIEnv *env, jobject) {
    std::string out;
    {
        std::lock_guard<std::mutex> lock(g_out_mutex);
        out.swap(g_output_buf);
    }
    return env->NewStringUTF(out.c_str());
}

JNIEXPORT jboolean JNICALL
Java_com_zink_kompakt_engine_ZinkEngine_isRunning(JNIEnv *env, jobject) {
    return g_running ? JNI_TRUE : JNI_FALSE;
}

} // extern "C"
