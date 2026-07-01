// zink_glk.cpp
// Minimal Glk implementation for Zink.
// Provides just enough for bocfel to initialise and run Z3/4/5 stories.
// Sound, graphics, and mouse events are stubbed as no-ops — Glk degrades
// gracefully when these return null/0.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <functional>
#include <jni.h>
#include <android/log.h>

#define LOG_TAG "ZinkGlk"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {
#include "glk/glk.h"
}

// ---------------------------------------------------------------------------
// Callbacks from JNI layer — set before glk_main() is called
// ---------------------------------------------------------------------------

// Called whenever bocfel wants to print a character to the main window.
static std::function<void(uint32_t)> g_put_char;
// Called whenever bocfel requests a line of input from the player.
static std::function<std::string()> g_get_line;

void zink_set_callbacks(std::function<void(uint32_t)> put_char,
                        std::function<std::string()> get_line) {
    g_put_char  = std::move(put_char);
    g_get_line  = std::move(get_line);
}

// ---------------------------------------------------------------------------
// Minimal window/stream structs — bocfel only needs one text buffer window
// ---------------------------------------------------------------------------

struct glk_window_struct  { uint32_t rock; };
struct glk_stream_struct  { uint32_t rock; bool is_current; };
struct glk_fileref_struct { uint32_t rock; std::string path; };

static glk_window_struct  g_mainwin  = { 0 };
static glk_stream_struct  g_mainstr  = { 0, true };
static glk_stream_struct  g_current  = { 0, true };

// ---------------------------------------------------------------------------
// Window management
// ---------------------------------------------------------------------------

winid_t glk_window_open(winid_t split, glui32 /*method*/, glui32 /*size*/,
                        glui32 wintype, glui32 rock) {
    if (split == nullptr && wintype == wintype_TextBuffer) {
        g_mainwin.rock = rock;
        return &g_mainwin;
    }
    // Upper window (status line) and graphics windows: return null.
    // bocfel handles null gracefully.
    return nullptr;
}

void    glk_window_close(winid_t win, stream_result_t *result)     { (void)win; (void)result; }
void    glk_window_clear(winid_t /*win*/)                          {}
void    glk_window_get_size(winid_t, glui32 *w, glui32 *h)        { if(w)*w=80; if(h)*h=24; }
void    glk_window_move_cursor(winid_t, glui32, glui32)            {}
void    glk_set_window(winid_t /*win*/)                            {}
winid_t glk_window_iterate(winid_t win, glui32 *rock)              {
    if (win == nullptr) { if (rock) *rock = g_mainwin.rock; return &g_mainwin; }
    return nullptr;
}
glui32  glk_window_get_rock(winid_t win)                           { return win ? win->rock : 0; }
glui32  glk_window_get_type(winid_t win)                           { return win ? wintype_TextBuffer : 0; }
winid_t glk_window_get_parent(winid_t)                             { return nullptr; }
winid_t glk_window_get_sibling(winid_t)                            { return nullptr; }
winid_t glk_window_get_root()                                      { return &g_mainwin; }
strid_t glk_window_get_stream(winid_t win)                         { return win ? &g_mainstr : nullptr; }
strid_t glk_window_get_echo_stream(winid_t)                        { return nullptr; }
void    glk_window_set_echo_stream(winid_t, strid_t)               {}
void    glk_window_set_arrangement(winid_t, glui32, glui32, winid_t) {}
void    glk_window_get_arrangement(winid_t, glui32*, glui32*, winid_t*) {}
void    glk_window_fill_rect(winid_t, glui32, glsi32, glsi32, glui32, glui32) {}
void    glk_window_erase_rect(winid_t, glsi32, glsi32, glui32, glui32) {}
void    glk_window_set_background_color(winid_t, glui32)           {}
glui32  glk_image_draw(winid_t, glui32, glsi32, glsi32)            { return 0; }
glui32  glk_image_draw_scaled(winid_t, glui32, glsi32, glsi32, glui32, glui32) { return 0; }
glui32  glk_image_get_info(glui32, glui32*, glui32*)               { return 0; }
void    glk_window_flow_break(winid_t)                             {}

// ---------------------------------------------------------------------------
// Stream / output
// ---------------------------------------------------------------------------

strid_t glk_stream_open_memory(char *buf, glui32 buflen, glui32, glui32 rock) {
    (void)buf; (void)buflen; (void)rock; return nullptr;
}
strid_t glk_stream_open_memory_uni(glui32*, glui32, glui32, glui32) { return nullptr; }
strid_t glk_stream_open_file(frefid_t, glui32, glui32)              { return nullptr; }
strid_t glk_stream_open_file_uni(frefid_t, glui32, glui32)          { return nullptr; }
void    glk_stream_close(strid_t str, stream_result_t *result)      { (void)str; (void)result; }
strid_t glk_stream_iterate(strid_t str, glui32 *rock)               {
    if (str == nullptr) { if (rock) *rock = g_mainstr.rock; return &g_mainstr; }
    return nullptr;
}
glui32  glk_stream_get_rock(strid_t str)    { return str ? str->rock : 0; }
void    glk_stream_set_position(strid_t, glsi32, glui32) {}
glui32  glk_stream_get_position(strid_t)    { return 0; }

void glk_stream_set_current(strid_t str)    { (void)str; }
strid_t glk_stream_get_current()            { return &g_current; }

void glk_put_char(unsigned char c) {
    if (g_put_char) g_put_char(static_cast<uint32_t>(c));
}
void glk_put_char_stream(strid_t, unsigned char c) {
    if (g_put_char) g_put_char(static_cast<uint32_t>(c));
}
void glk_put_char_uni(glui32 c) {
    if (g_put_char) g_put_char(c);
}
void glk_put_char_stream_uni(strid_t, glui32 c) {
    if (g_put_char) g_put_char(c);
}
void glk_put_string(char *s) {
    if (!s || !g_put_char) return;
    while (*s) g_put_char(static_cast<uint8_t>(*s++));
}
void glk_put_string_stream(strid_t, char *s)     { glk_put_string(s); }
void glk_put_string_uni(glui32 *s) {
    if (!s || !g_put_char) return;
    while (*s) g_put_char(*s++);
}
void glk_put_string_stream_uni(strid_t, glui32 *s) { glk_put_string_uni(s); }
void glk_put_buffer(char *buf, glui32 len) {
    if (!buf || !g_put_char) return;
    for (glui32 i = 0; i < len; i++) g_put_char(static_cast<uint8_t>(buf[i]));
}
void glk_put_buffer_stream(strid_t, char *buf, glui32 len)  { glk_put_buffer(buf, len); }
void glk_put_buffer_uni(glui32 *buf, glui32 len) {
    if (!buf || !g_put_char) return;
    for (glui32 i = 0; i < len; i++) g_put_char(buf[i]);
}
void glk_put_buffer_stream_uni(strid_t, glui32 *buf, glui32 len) { glk_put_buffer_uni(buf, len); }

glsi32 glk_get_char_stream(strid_t)          { return -1; }
glsi32 glk_get_char_stream_uni(strid_t)      { return -1; }
glui32 glk_get_line_stream(strid_t, char*, glui32) { return 0; }
glui32 glk_get_line_stream_uni(strid_t, glui32*, glui32) { return 0; }
glui32 glk_get_buffer_stream(strid_t, char*, glui32) { return 0; }
glui32 glk_get_buffer_stream_uni(strid_t, glui32*, glui32) { return 0; }

// ---------------------------------------------------------------------------
// Styles (no-ops — we render everything as plain text on e-ink)
// ---------------------------------------------------------------------------

void glk_set_style(glui32)               {}
void glk_set_style_stream(strid_t, glui32) {}
glui32 glk_style_distinguish(winid_t, glui32, glui32) { return 0; }
glui32 glk_style_measure(winid_t, glui32, glui32, glui32*) { return 0; }

// ---------------------------------------------------------------------------
// Events and input
// ---------------------------------------------------------------------------

void glk_request_line_event(winid_t win, char *buf, glui32 maxlen, glui32 /*initlen*/) {
    if (!win || !buf || !g_get_line) return;
    std::string line = g_get_line();
    glui32 len = static_cast<glui32>(line.size());
    if (len > maxlen - 1) len = maxlen - 1;
    std::memcpy(buf, line.c_str(), len);
    buf[len] = '\0';
}

void glk_request_line_event_uni(winid_t win, glui32 *buf, glui32 maxlen, glui32 /*initlen*/) {
    if (!win || !buf || !g_get_line) return;
    std::string line = g_get_line();
    glui32 len = static_cast<glui32>(line.size());
    if (len > maxlen - 1) len = maxlen - 1;
    for (glui32 i = 0; i < len; i++) buf[i] = static_cast<uint8_t>(line[i]);
    buf[len] = 0;
}

void glk_request_char_event(winid_t)         {}
void glk_request_char_event_uni(winid_t)     {}
void glk_cancel_char_event(winid_t)          {}
void glk_cancel_line_event(winid_t, event_t *ev) { if (ev) ev->type = evtype_None; }

void glk_select(event_t *ev) {
    if (!ev) return;
    // After a line-input request, bocfel calls glk_select to get the result.
    // We've already filled the buffer in glk_request_line_event, so just
    // report a completed line event on the main window.
    ev->type   = evtype_LineInput;
    ev->win    = &g_mainwin;
    ev->val1   = 0;
    ev->val2   = 0;
}

void glk_select_poll(event_t *ev) { if (ev) ev->type = evtype_None; }

void glk_request_mouse_event(winid_t)        {}
void glk_cancel_mouse_event(winid_t)         {}
void glk_request_hyperlink_event(winid_t)    {}
void glk_cancel_hyperlink_event(winid_t)     {}

// ---------------------------------------------------------------------------
// File references
// ---------------------------------------------------------------------------

frefid_t glk_fileref_create_temp(glui32 /*usage*/, glui32 rock) {
    auto *ref = new glk_fileref_struct();
    ref->rock = rock;
    ref->path = "/data/local/tmp/zink_save.sav";
    return ref;
}

frefid_t glk_fileref_create_by_name(glui32 /*usage*/, char *name, glui32 rock) {
    auto *ref = new glk_fileref_struct();
    ref->rock = rock;
    ref->path = name ? name : "zink_save.sav";
    return ref;
}

frefid_t glk_fileref_create_by_prompt(glui32 usage, glui32 /*fmode*/, glui32 rock) {
    return glk_fileref_create_temp(usage, rock);
}

frefid_t glk_fileref_create_from_fileref(glui32, frefid_t fref, glui32 rock) {
    auto *ref = new glk_fileref_struct();
    ref->rock = rock;
    ref->path = fref ? fref->path : "";
    return ref;
}

void    glk_fileref_destroy(frefid_t ref)             { delete ref; }
frefid_t glk_fileref_iterate(frefid_t, glui32*)       { return nullptr; }
glui32  glk_fileref_get_rock(frefid_t ref)            { return ref ? ref->rock : 0; }
glui32  glk_fileref_does_file_exist(frefid_t ref)     {
    if (!ref) return 0;
    FILE *f = std::fopen(ref->path.c_str(), "r");
    if (f) { std::fclose(f); return 1; }
    return 0;
}
void    glk_fileref_delete_file(frefid_t ref)         {
    if (ref) std::remove(ref->path.c_str());
}

strid_t glk_stream_open_file_by_path(const char *path, glui32 fmode, glui32 rock) {
    (void)path; (void)fmode; (void)rock; return nullptr;
}

// ---------------------------------------------------------------------------
// Timing / sound / misc (all no-ops)
// ---------------------------------------------------------------------------

void    glk_tick()                                    {}
void    glk_request_timer_events(glui32)              {}
glui32  glk_gestalt(glui32 /*sel*/, glui32)           { return 0; }
glui32  glk_gestalt_ext(glui32, glui32, glui32*, glui32) { return 0; }
void    glk_set_hyperlink(glui32)                     {}
void    glk_set_hyperlink_stream(strid_t, glui32)     {}

// Sound
glui32  glk_schannel_create(glui32)                   { return 0; }
void    glk_schannel_destroy(schanid_t)               {}
schanid_t glk_schannel_iterate(schanid_t, glui32*)    { return nullptr; }
glui32  glk_schannel_get_rock(schanid_t)              { return 0; }
glui32  glk_schannel_play(schanid_t, glui32)          { return 0; }
glui32  glk_schannel_play_ext(schanid_t, glui32, glui32, glui32) { return 0; }
void    glk_schannel_stop(schanid_t)                  {}
void    glk_schannel_set_volume(schanid_t, glui32)    {}
void    glk_sound_load_hint(glui32, glui32)           {}

// ---------------------------------------------------------------------------
// glk_exit — called by bocfel when the story ends or the player quits
// ---------------------------------------------------------------------------

void glk_exit() {
    LOGI("glk_exit called — story finished");
    // Don't call exit() — we're in a background thread on Android.
    // Just throw so the JNI layer can catch it and clean up.
    throw std::runtime_error("glk_exit");
}
