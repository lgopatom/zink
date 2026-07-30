package com.zink.kompakt

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.imePadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.mudita.mmd.components.divider.HorizontalDividerMMD
import com.mudita.mmd.components.text.TextMMD
import com.zink.kompakt.engine.ZinkEngine
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive

@Composable
fun GameScreen(
    storyPath: String,
    onGameOver: () -> Unit,
) {
    val lines = remember { mutableStateListOf<String>() }
    val listState = rememberLazyListState()
    var inputText by remember { mutableStateOf("") }
    var inputEnabled by remember { mutableStateOf(false) }

    // Shared text style — monospace, slightly larger than default body
    val gameTextStyle = TextStyle(
        fontFamily = FontFamily.Monospace,
        fontSize = 16.sp,
        color = MaterialTheme.colorScheme.onSurface,
    )

    DisposableEffect(storyPath) {
        ZinkEngine.start(storyPath)
        onDispose { }
    }

    LaunchedEffect(storyPath) {
        while (isActive) {
            val chunk = ZinkEngine.pollOutput()
            if (chunk.isNotEmpty()) {
                val incoming = chunk.split("\n")
                if (lines.isEmpty()) {
                    lines.addAll(incoming)
                } else {
                    lines[lines.lastIndex] += incoming.first()
                    lines.addAll(incoming.drop(1))
                }
                if (lines.isNotEmpty()) {
                    listState.animateScrollToItem(lines.lastIndex)
                }
                inputEnabled = true
            }

            if (!ZinkEngine.isRunning() && chunk.isEmpty()) {
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
                .imePadding()
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
                        style = gameTextStyle,
                        modifier = Modifier.fillMaxWidth(),
                    )
                }
            }

            HorizontalDividerMMD()

            // Input row — plain "> " prefix + invisible text field
            Row(
                verticalAlignment = Alignment.CenterVertically,
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 12.dp, vertical = 10.dp),
            ) {
                TextMMD(
                    text = ">",
                    style = gameTextStyle,
                    modifier = Modifier.padding(end = 6.dp),
                )
                BasicTextField(
                    value = inputText,
                    onValueChange = { inputText = it },
                    enabled = inputEnabled,
                    singleLine = true,
                    textStyle = gameTextStyle,
                    keyboardOptions = KeyboardOptions(imeAction = androidx.compose.ui.text.input.ImeAction.Send),
                    keyboardActions = KeyboardActions(
                        onSend = {
                            if (inputText.isNotEmpty() && inputEnabled) {
                                lines[lines.lastIndex] += inputText
                                lines.add("")
                                ZinkEngine.input(inputText)
                                inputText = ""
                                inputEnabled = false
                            }
                        }
                    ),
                    modifier = Modifier.fillMaxWidth(),
                )
            }
        }
    }
}
