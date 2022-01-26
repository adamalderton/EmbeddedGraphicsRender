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
	const Triangle3DStorage cube[NUM_TRIANGLES] = 
	{
		/* Front face. */
		{
			B,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{-L, -L, -L},
				{L, -L, -L},
				{-L, L, -L}
			}
		},
		{
			B,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{-L, L, -L},
				{L, -L, -L},
				{L, L, -L}
			}
		},
		/* Right-side face. */
		{
			G,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{L, -L, -L},
				{L, -L, L},
				{L, L, -L}
			}
		},
		{
			G,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{L, -L, L},
				{L, L, L},
				{L, L, -L}
			}
		},

		/* Top face. */
		{
			R,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{-L, L, L},
				{-L, L, -L},
				{L, L, -L}
			}
		},
		{
			R,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{L, L, L},
				{-L, L, L},
				{L, L, -L}
			}
		},

		/* Back face. */
		{
			B,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{L, L, L},
				{L, -L, L},
				{-L, L, L}
			}
		},
		{
			B,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{-L, L, L},
				{L, -L, L},
				{-L, -L, L}
			}
		},

		/* Left face. */
		{
			G,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{-L, L, -L},
				{-L, L, L},
				{-L, -L, L}
			}
		},
		{
			G,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{-L, L, -L},
				{-L, -L, L},
				{-L, -L, -L}
			}
		},
	
		/* Bottom face. */
		{
			R,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{-L, -L, L},
				{L, -L, L},
				{-L, -L, -L}
			}
		},
		{
			R,					/* Colour. */
			{					/* Vertices in -1.0 -> 1.0 space. */
				{-L, -L, -L},
				{L, -L, L},
				{L, -L, -L}
			}
		},
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
		Triangle2D tri2;

		uint32_t start_milliseconds = OSA_TimeGetMsec();
		uint32_t end_milliseconds;

		for (uint8_t j = 0; j < NUM_ROTATIONS; j++) {
			for (uint8_t rotation_num = 0; rotation_num < 10; rotation_num++) {
				
				/* Extract triangle values. */
				for (uint8_t tri_num = 0; tri_num < NUM_TRIANGLES; tri_num++) {

					tri3.colour = cube[tri_num].colour;

					tri3.vs[0][X] = cube[tri_num].vs[0][X];
					tri3.vs[0][Y] = cube[tri_num].vs[0][Y];
					tri3.vs[0][Z] = cube[tri_num].vs[0][Z];

					tri3.vs[1][X] = cube[tri_num].vs[1][X];
					tri3.vs[1][Y] = cube[tri_num].vs[1][Y];
					tri3.vs[1][Z] = cube[tri_num].vs[1][Z];

					tri3.vs[2][X] = cube[tri_num].vs[2][X];
					tri3.vs[2][Y] = cube[tri_num].vs[2][Y];
					tri3.vs[2][Z] = cube[tri_num].vs[2][Z];

					/*
						With the triangle extracted, we now rotate it a certain amount for the purposes of the demo.
						To do that, we need to define the two angles of rotation theta and phi, both analagous to their use in
						spherical coordinates. That is, phi is the azimuthal angle.

						These angles are used to collect values from the uint8_t sine_lookup table.
					*/
					rotate(&tri3, rotation_num);

					z_translate(&tri3);

					find_triangle_normal(&tri3);

					/*
						If we can see the correct face of the triangle, project and draw it.
						Can use any point on the triangle.

						This assumes that the camera lies at (0.0, 0.0, 0.0) and is directionless.
					*/
					if (dot_product_float_3d(tri3.normal, tri3.vs[0]) > 0.0) {
						project(tri3, &tri2);
						drawTriangle(frame, tri2);
					}
				}
				
				writeFrame(frame);

				RESET_FRAME(frame);
			}
		}

		end_milliseconds = OSA_TimeGetMsec();

		/* Milliseconds division can be truncated safely. */
		warpPrint("Average time per frame for %d frames: %dms.\n", NUM_ROTATIONS * 255, (end_milliseconds - start_milliseconds) / (NUM_ROTATIONS * 255));

	#endif

}

