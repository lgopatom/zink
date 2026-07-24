package com.zink.kompakt

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.PathEffect
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import com.mudita.mmd.ThemeMMD
import com.mudita.mmd.components.text.TextMMD
import com.zink.kompakt.engine.ZinkEngine

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            ThemeMMD {
                Surface {
                    ZinkApp()
                }
            }
        }
    }
}

// Simple two-screen navigation state
private sealed class Screen {
    object Library : Screen()
    data class Game(val path: String) : Screen()
}

@Composable
private fun ZinkApp() {
    var screen: Screen by remember { mutableStateOf(Screen.Library) }

    when (val s = screen) {
        is Screen.Library -> LibraryScreen(
            onStorySelected = { path -> screen = Screen.Game(path) }
        )
        is Screen.Game -> GameScreen(
            storyPath = s.path,
            onGameOver = { screen = Screen.Library }
        )
    }
}

// ---------------------------------------------------------------------------
// Temporary story list — replace with real scanner output in Milestone 3.
// Each entry is a Pair(displayTitle, absolutePath).
// Put a real .z5 file on the Kompakt at this path to test:
//   adb push Trinity.z4 /sdcard/Trinity.z4
// then update the path below to match.
// ---------------------------------------------------------------------------
private val placeholderStories = listOf(
    Pair("Trinity",                    "/sdcard/Trinity.z4"),
                                        Pair("A Mind Forever Voyaging",    "/sdcard/AMFV.z5"),
                                        Pair("Curses",                     "/sdcard/Curses.z5"),
                                        Pair("Anchorhead",                 "/sdcard/Anchorhead.z5"),
)

@Composable
private fun LibraryScreen(onStorySelected: (String) -> Unit) {
    Column(modifier = Modifier.fillMaxSize()) {
        TextMMD(
            text = "Zink — Story Library (bocfel ${ZinkEngine.version()})",
                style = MaterialTheme.typography.headlineLarge,
                modifier = Modifier.padding(16.dp),
        )

        DashedDivider()

        LazyColumn(modifier = Modifier.fillMaxSize()) {
            items(placeholderStories) { (title, path) ->
                TextMMD(
                    text = title,
                    style = MaterialTheme.typography.titleLarge,
                    modifier = Modifier
                    .fillMaxWidth()
                    .clickable { onStorySelected(path) }
                    .padding(horizontal = 16.dp, vertical = 14.dp),
                )
                DashedDivider()
            }
        }
    }
}

@Composable
private fun DashedDivider(
    modifier: Modifier = Modifier,
    thickness: Dp = 0.75.dp,
    dashLength: Dp = 5.dp,
    gapLength: Dp = 5.dp,
    color: Color = MaterialTheme.colorScheme.outlineVariant,
) {
    Canvas(
        modifier = modifier
        .fillMaxWidth()
        .height(thickness),
    ) {
        drawLine(
            color = color,
            start = Offset(0f, size.height / 2),
                 end = Offset(size.width, size.height / 2),
                 strokeWidth = thickness.toPx(),
                 pathEffect = PathEffect.dashPathEffect(
                     floatArrayOf(dashLength.toPx(), gapLength.toPx()),
                                                        0f,
                 ),
        )
    }
}
