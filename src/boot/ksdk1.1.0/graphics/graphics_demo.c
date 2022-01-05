#include <stdint.h>

#include "warp.h"

#include "devSSD1331.h"
#include "draw_triangle.h"
#include "draw_line.h"

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

void graphicsDemo(void)
{
	/* Initialise frame entirely to 0. */
	uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS];
	for (uint8_t i = 0; i < FRAME_TRUE_ROWS; i++) {
		for (uint8_t j = 0; j < FRAME_TRUE_COLS; j++){
			frame[i][j] = 0;
		}
	}

	devSSD1331init();

	// drawHorizontalLine(frame, 1, 1, 34, G, MAX_RELATIVE_INTENSITY);

	// drawHorizontalLine(frame, 34, 1, 34, R, MAX_RELATIVE_INTENSITY);

	writeFrame(frame);

	warpPrint("Writeframe complete!\n");
}