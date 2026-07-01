cmake_minimum_required(VERSION 3.22.1)
project(zink)

# Collect all bocfel .cpp files
file(GLOB BOCFEL_SOURCES "bocfel/*.cpp")

add_library(
    zink
    SHARED
    ${BOCFEL_SOURCES}
    zink_glk.cpp
    zink_jni.cpp
)

target_include_directories(zink PRIVATE
    bocfel
    glk
)

target_compile_definitions(zink PRIVATE
    # Tell bocfel to use Glk for I/O (not its standalone terminal mode)
    ZTERP_GLK
    # We don't need Glk Blorb support for plain .z? files
    # ZTERP_GLK_BLORB   # leave off for now
    # No ICU unicode library — bocfel degrades gracefully
    # ZTERP_ICU         # leave off
)

target_compile_options(zink PRIVATE
    -std=c++17
    -fexceptions
    -Wall
    -Wno-unused-parameter
)

# Link Android log library for LOGI/LOGE macros
target_link_libraries(zink
    android
    log
)
