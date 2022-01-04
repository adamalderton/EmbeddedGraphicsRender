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
	// uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS] = {{0}};

	devSSD1331init();

	warpPrint("devSSD1331init complete!\n");

	writeFrame();

	warpPrint("Writeframe complete!\n");
}