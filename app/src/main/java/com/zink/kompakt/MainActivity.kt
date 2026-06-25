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
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.PathEffect
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import com.mudita.mmd.ThemeMMD
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
            style = MaterialTheme.typography.headlineLarge,
            modifier = Modifier.padding(16.dp),
        )

        DashedDivider()

        LazyColumn(modifier = Modifier.fillMaxSize()) {
            items(placeholderStories) { title ->
                TextMMD(
                    text = title,
                    style = MaterialTheme.typography.titleLarge,
                    modifier = Modifier
                    .fillMaxWidth()
                    .clickable { /* real engine launch comes in Milestone 2 */ }
                    .padding(horizontal = 16.dp, vertical = 14.dp),
                )
                DashedDivider()
            }
        }
    }
}

/**
 * A thin dashed (perforated) divider — lighter on e-ink than a solid line,
 * since roughly half the row width carries no ink at all.
 */
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
