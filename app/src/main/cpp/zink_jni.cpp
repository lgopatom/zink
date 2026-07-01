// zink_jni.cpp
// JNI entry points exposed to Kotlin.
// Milestone 2 walking skeleton: just zinkVersion() to prove the NDK pipeline.
// zinkStart() / zinkInput() come in the next pass once this compiles.

#include <jni.h>
#include <string>
#include <android/log.h>

// Pull in bocfel's version string
#include "bocfel/zterp.h"

#define LOG_TAG "ZinkJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)

extern "C" {

/**
 * Returns bocfel's version string, e.g. "2.4".
 * Called from Kotlin as: ZinkEngine.version()
 */
JNIEXPORT jstring JNICALL
Java_com_zink_kompakt_engine_ZinkEngine_version(JNIEnv *env, jobject /*thiz*/) {
    LOGI("zinkVersion called");
    return env->NewStringUTF(ZTERP_VERSION);
}

} // extern "C"
