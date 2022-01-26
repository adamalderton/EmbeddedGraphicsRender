#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

/* Aspect ratio. */
#define A__ ( (float) FRAME_NUM_ROWS / (float) FRAME_NUM_COLS )

/*
    NEEDS TO BE PRE-CALCULATED EXTERNALLY FROM THE PROGRAM.

    This is to prevent inefficient math.h functions from being included by the linker.
    B is the field of view scaling factor, \frac{1}{tan(\frac{\theta}{2})} where \theta is the field of view angle.
    The below value is for \theta = \pi / 2.
*/
#define B__ 1.0

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

/*
    Rotates a 3D triangle in 3D space. 'rotation_num' can be
    thought of as a similar quantity to an angle. However, instead of
    ranging from 0 -> 360 or 0 -> 2\pi, it ranges from 0->255 such that it
    matches up with the uint8_t datatype and the sine lookup table implemented
    in projection.c.
*/
void rotate(Triangle3D *tri3, uint8_t rotation_num);

/*
    Perspectively projects the 3D tri3 into 2D, storing the result in tri2.
    During this process, the relative intensity that the 2D triangle should be
    displayed with is calculated.
*/
void project(Triangle3D tri3, Triangle2D *tri2);

/* Find the normal of a triangle defined in 3D. */
void find_triangle_normal(Triangle3D *tri3);