#include "projection.h"

/*
Make frame static
Maybe store triangles in __flash ROM?
    Can calculate rotation from scratch every time
    Only store one actual 'triangle' in ram at any one time
two static single precision matrices:
    rotation matrix
    const projection matrix
rotation sin:
    __flash lookup table (see website)
Define triangle points:
    SIGNED int8_t, normalised to -127 to +127
    Centre on 0,0 for easy rotation
    probably define cube to be between -100 and 100 to account for hypotenuse
Projection:
    Operate rotation matrix on 3D triangle
        Define in terms of rotations per sec (<< 1) using OSA_GetMSec()
    Project to 2D (store 2D vertices as signed?)
    Translate into z axis
    Translate so bottom left is 0,0, cast to unsigned
    Pass to drawTriangle function
Intensity:
    Maybe store normal as float?
    Only store one normal at a time? i.e - find it every time as opposed to rotating it every time?
    Find cos theta using dot product formula (includes dividing by magnitude)
    Const lookup table:
        Negative => not rendered = 0 (new attribute of triangle 2D? Maybe a return value?)
        0.0 -> 0.4 => Intensity = 1
        0.4 -> 0.8 => Intensity = 2
        0.8 -> 1.0 => Intensity = 3
DONT DO PAINTERS ALGORITHM / Concave shapes - not too embedded related!
Do another convex shape, something like this? <=>
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
void z_translate(Triangle3D *tri3)
{
    tri3->vs[0][Z] += Z_TRANSLATION;
	tri3->vs[1][Z] += Z_TRANSLATION;
	tri3->vs[2][Z] += Z_TRANSLATION;
}

void rotate(Triangle3D *tri3, uint8_t rotation_num)
{
    /* Used to temporarily hold result of rotation. */
    float temp[3];

    /* Extract sin(angle) and cos(angle) for both theta and phi. */
    float sin_theta = SIN_UINT8(ROTATION_RATE_THETA, rotation_num);
    float cos_theta = COS_UINT8(ROTATION_RATE_THETA, rotation_num);

    float sin_phi = SIN_UINT8(ROTATION_RATE_PHI, rotation_num);
    float cos_phi = COS_UINT8(ROTATION_RATE_PHI, rotation_num);

    /*
        Operate rotation matrix on vertices of tri3.

        Combining usual Z and X axis rotations, and defining A = cos(theta), B = sin(theta),
        C = cos(phi) and D = sin(phi), we find the following:
          C   -D    0
        ( AD  AC   -B )
          0   BD    A
    */
    for (uint8_t i = 0; i < 3; i++) {
        temp[X] = (cos_phi * tri3->vs[i][X]) + (-sin_phi * tri3->vs[i][Y]);
        temp[Y] = (cos_theta * sin_phi * tri3->vs[i][X]) + (cos_theta * cos_phi * tri3->vs[i][Y]) + (-sin_theta * tri3->vs[i][Z]);
        temp[Z] = (sin_theta * sin_phi * tri3->vs[i][Y]) + (cos_theta * tri3->vs[i][Z]);

        tri3->vs[i][X] = temp[X];
        tri3->vs[i][Y] = temp[Y];
        tri3->vs[i][Z] = temp[Z];
    }
}

Triangle2D project(Triangle3D tri3)
{
    Triangle2D tri2;

    tri2.colour = tri3.colour;

    /*
        Project the coordinates using a sparse-matrix multiplication of the
        projection matrix. Inspired by discussion at https://community.onelonecoder.com/.
        The projected coordinates can be stored in tri3 for now for some further processing. Particularly,
        calculating normals in the interest of relative intensity.
    */
    for (uint8_t i = 0; i < 3; i++) {
        tri3.vs[i][X] = ( (A__ * B__ * tri3.vs[i][X]) / (tri3.vs[i][Z]) );
        tri3.vs[i][Y] = ( (B__ * tri3.vs[i][Y]) / (tri3.vs[i][Z]) );
        tri3.vs[i][Z] = (C__ * tri3.vs[i][Z]) - (C__ * Z_NEAR);

        /*
            Finally, generate the 2D Triangle value. multiply it by FRAME_NUM_COLS to get it into
            pixel space, then translate such that 0,0 is no longer in the centre but the bottom left.
            Finally cast to uint8_t. Truncation is okay as its consistent.
        */
        tri2.vs[i][X] = (uint8_t) ( (tri3.vs[i][X] * (float) FRAME_NUM_COLS) + (FRAME_NUM_COLS / 2) );
        tri2.vs[i][Y] = (uint8_t) ( (tri3.vs[i][Y] * (float) FRAME_NUM_ROWS) + (FRAME_NUM_ROWS / 2) );
    }

    /* Next, calculate the relative intensity. */
    tri2.relative_intensity = MAX_RELATIVE_INTENSITY;

    return tri2;
}