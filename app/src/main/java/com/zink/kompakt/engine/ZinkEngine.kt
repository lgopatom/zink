package com.zink.kompakt.engine

/**
 * Kotlin interface to the native libzink.so.
 *
 * All functions except version() must be called from a non-main thread,
 * since start() launches a blocking interpreter loop and pollOutput()/input()
 * are designed to be called from a coroutine.
 */
object ZinkEngine {

    init {
        System.loadLibrary("zink")
    }

    /** Returns bocfel's version string, e.g. "2.1". */
    external fun version(): String

    /**
     * Starts the interpreter on a native background thread for the given
     * story file path. Returns true if started successfully.
     * Call only once per story session — call stop() before starting again.
     */
    external fun start(path: String): Boolean

    /**
     * Sends a line of text input to the interpreter (as if the player typed
     * it and pressed Enter). Unblocks the interpreter if it's waiting for input.
     */
    external fun input(line: String)

    /**
     * Drains any text output the interpreter has produced since the last call.
     * Returns an empty string if nothing is waiting.
     * Call this periodically (e.g. every 100ms) from a coroutine.
     */
    external fun pollOutput(): String

    /** Returns true if the interpreter thread is currently running. */
    external fun isRunning(): Boolean
}
