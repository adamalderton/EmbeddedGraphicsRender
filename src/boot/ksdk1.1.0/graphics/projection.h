#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

#define Z_FAR 100.0
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

#define SIN_UINT8(rot_rate, rot_num) \
	(float) ( (((float) sine_lookup[(rot_rate * rot_num) % 255]) - 128.0) / 128.0 )

#define COS_UINT8(rot_rate, rot_num) \
	(float) ( (((float) sine_lookup[(64 - (rot_rate * rot_num)) % 255]) - 128.0) / 128.0 )

void z_translate(Triangle3D *tri3);
void rotate(Triangle3D *tri3, uint8_t rotation_num);
Triangle2D project(Triangle3D);