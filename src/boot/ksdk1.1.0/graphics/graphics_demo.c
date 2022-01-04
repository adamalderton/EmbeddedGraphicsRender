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

	// for (uint8_t x = 0; x < 10; x++) {
	// 	for (uint8_t y = 0; y < FRAME_NUM_COLS; y++){
	// 		drawPixel(frame, x, y, B, MAX_RELATIVE_INTENSITY);
	// 	}
	// }

	// for (uint8_t x = 10; x < 15; x++) {
	// 	for (uint8_t y = 0; y < FRAME_NUM_COLS; y++){
	// 		drawPixel(frame, x, y, R, MAX_RELATIVE_INTENSITY);
	// 	}
	// }
	
	for (uint8_t x = 0; x < 40; x++) {
		drawPixel(frame, x, 0, B, MAX_RELATIVE_INTENSITY);
	}

	devSSD1331init();

	warpPrint("devSSD1331init complete!\n");

	writeFrame(frame);

	warpPrint("Writeframe complete!\n");
}