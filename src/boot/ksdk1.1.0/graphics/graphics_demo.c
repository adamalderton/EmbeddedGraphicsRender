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

	for (uint8_t x = 0; x < FRAME_NUM_COLS; x++) {
		for (uint8_t y = 0; y < 5; y++) {
			drawPixel(frame, x, y, B, 0);
		}
	}

	for (uint8_t x = 0; x < FRAME_NUM_COLS; x++) {
		for (uint8_t y = 5; y < 10; y++) {
			drawPixel(frame, x, y, B, 1);
		}
	}

	for (uint8_t x = 0; x < FRAME_NUM_COLS; x++) {
		for (uint8_t y = 10; y < 15; y++) {
			drawPixel(frame, x, y, B, 2);
		}
	}

	for (uint8_t x = 0; x < FRAME_NUM_COLS; x++) {
		for (uint8_t y = 15; y < 20; y++) {
			drawPixel(frame, x, y, B, 3);
		}
	}

	writeFrame(frame);

	warpPrint("Writeframe complete!\n");
}