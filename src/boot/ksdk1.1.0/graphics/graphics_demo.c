#include <stdint.h>

#include "devSSD1331.h"
#include "draw_triangle.h"
#include "draw_line.h"
#include "projection.h"
#include "warp.h"

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

#if (SPINNING_MULTICOLOUR_CUBE_DEMO)


	/*
		Cube constructed in terms of right hand rule triangles.
		Stored in FLASH/ROM as of course this is memory intenstive.
		We can read and rotate the vertices one by one each time, rather than store all 
		the rotated versions for every frame.
	*/
	const float cube[1][3][3] = 
	{
		{
			{-0.5, -0.5, 0.0},
			{0.7, 0.1, 0.0},
			{-0.1, 0.8, 0.0}
		}
	};

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

	#if (SPINNING_MULTICOLOUR_CUBE_DEMO)

		Triangle3D tri3;

		tri3.colour = R;

		tri3.vs[0][X] = cube[0][0][X];
		tri3.vs[0][Y] = cube[0][0][Y];
		tri3.vs[0][Z] = cube[0][0][Z];

		tri3.vs[1][X] = cube[0][1][X];
		tri3.vs[1][Y] = cube[0][1][Y];
		tri3.vs[1][Z] = cube[0][1][Z];

		tri3.vs[2][X] = cube[0][2][X];
		tri3.vs[2][Y] = cube[0][2][Y];
		tri3.vs[2][Z] = cube[0][2][Z];

		// rotate(&tri3, theta, phi);

		/*
			With the triangle rotated, translate it into the Z axis such that it is NOT
			in the region of (0, 0, 0).
		*/
		tri3.vs[0][Z] += Z_TRANSLATION;
		tri3.vs[1][Z] += Z_TRANSLATION;
		tri3.vs[2][Z] += Z_TRANSLATION;

		Triangle2D tri2 = project(tri3);

		printf("(%d, %d)\n", tri2.vs[0][X], tri2.vs[0][Y]);
		printf("(%d, %d)\n", tri2.vs[1][X], tri2.vs[1][Y]);
		printf("(%d, %d)\n", tri2.vs[2][X], tri2.vs[2][Y]);
		// drawTriangle(frame, project(tri3));

	#endif

}

