#include <stdint.h>

#include "devSSD1331.h"
#include "draw_triangle.h"
#include "draw_line.h"
#include "warp.h"

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

/*
    Lookup table for the sine function with the unsigned 8 bit integer
    datatype. Efforts should be made to store this in FLASH ROM, this is done using the const keyword
    with the ARM compiler but may be done otherwise with other compilers.

	Made using https://www.daycounter.com/Calculators/Sine-Generator-Calculator2.phtml.
*/
const uint8_t sine_lookup[256] =
{
128,131,134,137,140,143,146,149,
152,155,158,162,165,167,170,173,
176,179,182,185,188,190,193,196,
198,201,203,206,208,211,213,215,
218,220,222,224,226,228,230,232,
234,235,237,238,240,241,243,244,
245,246,248,249,250,250,251,252,
253,253,254,254,254,255,255,255,
255,255,255,255,254,254,254,253,
253,252,251,250,250,249,248,246,
245,244,243,241,240,238,237,235,
234,232,230,228,226,224,222,220,
218,215,213,211,208,206,203,201,
198,196,193,190,188,185,182,179,
176,173,170,167,165,162,158,155,
152,149,146,143,140,137,134,131,
128,124,121,118,115,112,109,106,
103,100,97,93,90,88,85,82,
79,76,73,70,67,65,62,59,
57,54,52,49,47,44,42,40,
37,35,33,31,29,27,25,23,
21,20,18,17,15,14,12,11,
10,9,7,6,5,5,4,3,
2,2,1,1,1,0,0,0,
0,0,0,0,1,1,1,2,
2,3,4,5,5,6,7,9,
10,11,12,14,15,17,18,20,
21,23,25,27,29,31,33,35,
37,40,42,44,47,49,52,54,
57,59,62,65,67,70,73,76,
79,82,85,88,90,93,97,100,
103,106,109,112,115,118,121,124
};

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

	#if (SPINNING_MULTICOLOUR_CUBE_DEMO)

		for (uint8_t i = 0; i < 255; i++) {
			warpPrint("(%d, %d)\n", i, sine_lookup[i]);
			warpPrint("\n\n");
			OSA_TimeDelay(1000);
		}

	#endif

}

