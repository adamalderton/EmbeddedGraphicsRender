#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

#define Z_FAR 1000.0
#define Z_NEAR 1.0

/*
	A, B and C below are used in the projection of a 3D triangle to a 2D triangle.
	See community.onelonecoder.com for a description of a similar system (defined in 'a', 'F' and 'q' respectively).
*/

/* Aspect ratio. */
#define A__ ( (float) FRAME_NUM_ROWS / (float) FRAME_NUM_COLS )

/*
    NEEDS TO BE PRE-CALCULATED EXTERNALLY FROM THE PROGRAM.

    This is to prevent inefficient math.h functions from being included by the linker.
    B is the field of view scaling factor, \frac{1}{tan(\frac{\theta}{2})} where \theta is the field of view angle.
    The below value is for \theta = \pi / 2.
*/
#define B__ 1.0

#define C__ (Z_FAR / (Z_FAR - Z_NEAR))

#define SIN_UINT8(x) \
	(float) ( (((float) sine_lookup[x % 255]) - 128.0) / 128.0 )

#define COS_UINT8(x) \
	(float) ( (((float) sine_lookup[(uint8_t) (64 - x) % 255]) - 128.0) / 128.0 )

/*
    Translate a 3D triangle into the z axis such that is not centred around
    (0, 0, 0). Rendering at z = 0 is undefined and rendering at z < 1 will quickly ensure 
    that the triangle extends beyond the bound of the screen. Therefore,
    being that rendered shapes are defined in the range (-1.0 -> 1.0) in every axis, is is recommended
    that Z_TRANSLATION is no less than 2.0 such that points defined at z = -1.0 still render at least 
    at z = 1.0.
    Summary:
        Z_TRANSLATION *MUST* be greater than 1.0 (not inclusive)
        Z_TRANSLATION should be greater or equal to 2.0 to ensure vertices do not extend beyond the bounds of the screen.
*/
void z_translate(Triangle3D *tri3);

/* TODO: Docs. */
void rotate(Triangle3D *tri3, uint8_t rotation_num);

/* TODO: Docs. */
Triangle2D project(Triangle3D);

/*
    Find the normal of a triangle defined in 3D.

    This is an expensive function in both compute and memory.
*/
void find_triangle_normal(Triangle3D *tri3);