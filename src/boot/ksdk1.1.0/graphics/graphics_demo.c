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

#if (SPINNING_SQUARE_DEMO)

	const Triangle3DStorage square[NUM_TRIANGLES] = 
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
		}
	};
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

	/* Initialise screen. */
	devSSD1331init();

	#if (SPINNING_SQUARE_DEMO)

		Triangle3D tri3;
		Triangle2D tri2;

		uint32_t start_milliseconds = OSA_TimeGetMsec();
		uint32_t end_milliseconds;

		for (uint8_t j = 0; j < NUM_ROTATIONS; j++) {
			for (uint8_t rotation_num = 0; rotation_num < 255; rotation_num++) {
				for (uint8_t tri_num = 0; tri_num < NUM_TRIANGLES; tri_num++) {

					tri3.colour = square[tri_num].colour;

					tri3.vs[0][X] = square[tri_num].vs[0][X];
					tri3.vs[0][Y] = square[tri_num].vs[0][Y];
					tri3.vs[0][Z] = square[tri_num].vs[0][Z];

					tri3.vs[1][X] = square[tri_num].vs[1][X];
					tri3.vs[1][Y] = square[tri_num].vs[1][Y];
					tri3.vs[1][Z] = square[tri_num].vs[1][Z];

					tri3.vs[2][X] = square[tri_num].vs[2][X];
					tri3.vs[2][Y] = square[tri_num].vs[2][Y];
					tri3.vs[2][Z] = square[tri_num].vs[2][Z];

					/*
						With the triangle extracted, we now rotate it a certain amount for the purposes of the demo.
						To do that, we need to define the two angles of rotation theta and phi, both analagous to their use in
						spherical coordinates. That is, phi is the azimuthal angle.

						These angles are used to collect values from the uint8_t sine_lookup table.
					*/
					rotate(&tri3, rotation_num);

					z_translate(&tri3);

					find_triangle_normal(&tri3);

					/* In this demo, we can see both sides of the square. */
					project(tri3, &tri2);
					drawTriangle(frame, tri2);
				}

				writeFrame(frame);

				RESET_FRAME(frame);
			}
		}

		end_milliseconds = OSA_TimeGetMsec();

		/* Milliseconds division can be truncated safely. */
		warpPrint("Average time per frame for %d frames: %dms.\n", NUM_ROTATIONS * 255, (end_milliseconds - start_milliseconds) / (NUM_ROTATIONS * 255));

	#elif (SPINNING_MULTICOLOUR_CUBE_DEMO)

		Triangle3D tri3;
		Triangle2D tri2;

		uint32_t start_milliseconds = OSA_TimeGetMsec();
		uint32_t end_milliseconds;

		for (uint8_t j = 0; j < NUM_ROTATIONS; j++) {
			for (uint8_t rotation_num = 0; rotation_num < 255; rotation_num++) {
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


	#elif (TRIANGLES_VS_FRAMERATE_DEMO)

		Triangle3D tri3;
		Triangle2D tri2;

		uint32_t start_milliseconds = OSA_TimeGetMsec();
		uint32_t end_milliseconds;

		for (uint16_t num_tris = START_TRIANGLES; num_tris <= END_TRIANGLES; num_tris += STEP_TRIANGLES) {
			for (uint16_t frame_num = 0; frame_num < FRAMES_PER_STEP; frame_num++) {
				
				for (uint16_t tri_num = 0; tri_num < num_tris; tri_num++) {
					/* Vary colour so something can be seen on screen. */
					tri3.colour = (uint8_t) (frame_num % 3) + 1;

					/* Re-populate tri3 each time to mimic more realistic rendering demo. */
					tri3.vs[0][X] = -0.5;
					tri3.vs[0][Y] = -0.5;
					tri3.vs[0][Z] = -0.5;

					tri3.vs[1][X] = 0.5;
					tri3.vs[1][Y] = -0.5;
					tri3.vs[1][Z] = -0.5;

					tri3.vs[2][X] = -0.5;
					tri3.vs[2][Y] = 0.5;
					tri3.vs[2][Z] = -0.5;

					/* The following function calls won't really do much but we need to take into account their processing. */
					// rotate(&tri3, 0);
					
					z_translate(&tri3);

					find_triangle_normal(&tri3);

					project(tri3, &tri2);
					drawTriangle(frame, tri2);
				}
			
				writeFrame(frame);
				RESET_FRAME(frame);
			}

		end_milliseconds = OSA_TimeGetMsec();

		/* Milliseconds division can be truncated safely. */
		warpPrint("Average time per frame for %d frames rendering %d triangles: %dms.\n", FRAMES_PER_STEP, num_tris, (end_milliseconds - start_milliseconds) / FRAMES_PER_STEP);

		start_milliseconds = end_milliseconds;
	}

	#endif

}

