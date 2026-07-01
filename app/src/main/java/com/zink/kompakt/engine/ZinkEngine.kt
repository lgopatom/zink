package com.zink.kompakt.engine

/**
 * Kotlin interface to the native libzink.so.
 *
 * Milestone 2 walking skeleton: just version() to confirm the NDK build
 * pipeline works end-to-end. zinkStart() / zinkInput() come next.
 */
object ZinkEngine {

    init {
        System.loadLibrary("zink")
    }

    /** Returns bocfel's version string, e.g. "2.4". */
    external fun version(): String
}
