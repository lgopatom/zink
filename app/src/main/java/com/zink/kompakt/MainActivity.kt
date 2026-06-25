package com.zink.kompakt

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Surface
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.mudita.mmd.ThemeMMD
import com.mudita.mmd.components.buttons.ButtonMMD
import com.mudita.mmd.components.text.TextMMD

/**
 * Milestone 1: prove the build/CI/sideload pipeline end-to-end with a fake
 * library list, before the real bocfel engine and storage scanner are wired in.
 */
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            ThemeMMD {
                Surface {
                    LibraryScreen()
                }
            }
        }
    }
}

private val placeholderStories = listOf(
    "Trinity.z4",
    "A Mind Forever Voyaging.z5",
    "Curses.z5",
    "Anchorhead.z5",
)

@Composable
private fun LibraryScreen() {
    Column(modifier = Modifier.fillMaxSize()) {
        TextMMD(
            text = "Zink — Story Library",
            modifier = Modifier.padding(16.dp),
        )

        LazyColumn(
            modifier = Modifier.fillMaxSize(),
            contentPadding = PaddingValues(horizontal = 16.dp, vertical = 8.dp),
            verticalArrangement = Arrangement.spacedBy(8.dp),
        ) {
            items(placeholderStories) { title ->
                ButtonMMD(onClick = { /* real engine launch comes in Milestone 2 */ }) {
                    TextMMD(text = title)
                }
            }
        }
    }
}
