package com.zink.kompakt

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.imePadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Surface
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp
import com.mudita.mmd.ThemeMMD
import com.mudita.mmd.components.text.TextMMD
import com.zink.kompakt.engine.ZinkEngine
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive

/**
 * The IF play screen. Launched when the player taps a story title.
 *
 * Layout:
 *   - Scrolling transcript of game output (top, fills available space)
 *   - Single-line text input field (bottom, above keyboard)
 *
 * The interpreter runs on a native thread. We poll for output every 100ms
 * via a coroutine and append it to the transcript state list.
 */
@Composable
fun GameScreen(
    storyPath: String,
    onGameOver: () -> Unit,
) {
    // Transcript lines — each entry is a chunk of text from the interpreter.
    // We split on newlines when appending so the lazy list scrolls smoothly.
    val lines = remember { mutableStateListOf<String>() }
    val listState = rememberLazyListState()
    var inputText by remember { mutableStateOf("") }
    var inputEnabled by remember { mutableStateOf(false) }

    // Start the interpreter when this screen first appears.
    DisposableEffect(storyPath) {
        ZinkEngine.start(storyPath)
        onDispose {
            // Nothing to explicitly stop — the native thread exits when
            // glk_exit() is called or the story ends naturally.
        }
    }

    // Poll for output every 100ms and append to transcript.
    LaunchedEffect(storyPath) {
        while (isActive) {
            val chunk = ZinkEngine.pollOutput()
            if (chunk.isNotEmpty()) {
                // Split into lines but keep empty lines (paragraph breaks).
                val incoming = chunk.split("\n")
                if (lines.isEmpty()) {
                    lines.addAll(incoming)
                } else {
                    // Append first chunk to the last incomplete line.
                    lines[lines.lastIndex] += incoming.first()
                    lines.addAll(incoming.drop(1))
                }
                // Scroll to bottom whenever new output arrives.
                if (lines.isNotEmpty()) {
                    listState.animateScrollToItem(lines.lastIndex)
                }
                // Enable input once the interpreter has produced its first output.
                inputEnabled = true
            }

            if (!ZinkEngine.isRunning() && chunk.isEmpty()) {
                // Interpreter finished — brief pause then return to library.
                delay(2000)
                onGameOver()
                break
            }

            delay(100)
        }
    }

    Surface(modifier = Modifier.fillMaxSize()) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .imePadding()  // Shift content up when keyboard appears
        ) {
            // Scrolling transcript
            LazyColumn(
                state = listState,
                modifier = Modifier
                    .weight(1f)
                    .fillMaxWidth()
                    .padding(horizontal = 12.dp, vertical = 8.dp),
            ) {
                items(lines.size) { index ->
                    TextMMD(
                        text = lines[index],
                        style = MaterialTheme.typography.bodyMedium.copy(
                            fontFamily = FontFamily.Monospace,
                        ),
                        modifier = Modifier.fillMaxWidth(),
                    )
                }
            }

            // Input field — always visible at bottom
            OutlinedTextField(
                value = inputText,
                onValueChange = { inputText = it },
                enabled = inputEnabled,
                singleLine = true,
                placeholder = {
                    TextMMD(
                        text = if (inputEnabled) "> " else "Loading…",
                        style = MaterialTheme.typography.bodyMedium,
                    )
                },
                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Send),
                keyboardActions = KeyboardActions(
                    onSend = {
                        if (inputText.isNotEmpty() && inputEnabled) {
                            // Echo the command into the transcript.
                            lines[lines.lastIndex] += inputText
                            lines.add("")
                            ZinkEngine.input(inputText)
                            inputText = ""
                            // Disable input until the interpreter asks again.
                            inputEnabled = false
                        }
                    }
                ),
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 12.dp, vertical = 8.dp),
            )
        }
    }
}
