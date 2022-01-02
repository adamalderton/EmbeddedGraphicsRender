#include <stdint.h>

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
	uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS] = {{{0}}};

	uint8_t rgb[FRAME_NUM_COLOURS];

	uint8_t point_0[2] = {10, 10};
	uint8_t point_1[2] = {85, 54};

	rgb[R] = 0;
	rgb[G] = 0;
	rgb[B] = 1;

	devSSD1331init();

	drawLine(frame, point_0, point_1, rgb);

	writeFrame(frame);
}