#include <stdint.h>

#include "devSSD1331.h"
#include "draw_triangle.h"
#include "draw_line.h"
#include "warp.h"

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

	#if (OVERLAPPING_2D_TRIANGLES_DEMO)

		warpPrint("Beginning overlapping triangles demo...\n");

		uint16_t iterations;
		uint32_t start_milliseconds = OSA_TimeGetMsec();
		uint32_t milliseconds;

		Triangle2D tri;

		for (uint8_t n = 0; n < OVERLAPPING_2D_TRIANGLES_DEMO_ITERATIONS; n++) {
			RESET_FRAME(frame);

			tri.colour = (n % 4) + 1;
			tri.relative_intensity = 1;

			tri.vs[0][X] = 4; tri.vs[0][Y] = 0;
			tri.vs[1][X] = 35; tri.vs[1][Y] = 27;
			tri.vs[2][X] = 1; tri.vs[2][Y] = 35;

			drawTriangle(frame, tri);

			tri.colour = G;
			tri.relative_intensity = 3;
			tri.vs[0][X] = 20; tri.vs[0][Y] = 0;
			tri.vs[1][X] = 35; tri.vs[1][Y] = 16;
			tri.vs[2][X] = 26; tri.vs[2][Y] = 35;

			drawTriangle(frame, tri);

			writeFrame(frame);
		}

		milliseconds = OSA_TimeGetMsec();

		warpPrint("Milliseconds per frame: %d\n", (milliseconds - start_milliseconds) / OVERLAPPING_2D_TRIANGLES_DEMO_ITERATIONS);

	#endif
}

