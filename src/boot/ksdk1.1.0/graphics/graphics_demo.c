#include <stdint.h>

#include "warp.h"

#include "devSSD1331.h"
#include "draw_triangle.h"
#include "draw_line.h"
#include <string.h>   /* To time calculation execution. */

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

void graphicsDemo(void)
{
	/* Initialise frame entirely to 0. */
	uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS];
	RESET_FRAME(frame);

	devSSD1331init();

	uint8_t point_0[2] = {1, 1};
	uint8_t point_1[2] = {20, 1};
	drawLine(frame, point_0, point_1, B, MAX_RELATIVE_INTENSITY);

	point_0[X] = 1; point_0[Y] = 1;
	point_1[X] = 20; point_1[Y] = 30;
	drawLine(frame, point_0, point_1, G, MAX_RELATIVE_INTENSITY);

	point_0[X] = 20; point_0[Y] = 35;
	point_1[X] = 1; point_1[Y] = 5;
	drawLine(frame, point_0, point_1, R, MAX_RELATIVE_INTENSITY);

	point_0[X] = 33; point_0[Y] = 35;
	point_1[X] = 33; point_1[Y] = 1;
	drawLine(frame, point_0, point_1, R, MAX_RELATIVE_INTENSITY);


	// for (int i = 0; i < FRAME_NUM_ROWS; i++) {
	// 	for (int j = 0; j < FRAME_NUM_COLS; j++) {
	// 		warpPrint("%d ", frame[i][j]);
	// 	}
	// 	warpPrint("\n");
	// }

	writeFrame(frame);

	warpPrint("done\n");
}