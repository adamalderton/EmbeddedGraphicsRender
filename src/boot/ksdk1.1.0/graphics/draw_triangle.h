#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

typedef struct {
    int16_t octant;     /* Octant of line. Either 0 or 1. */
    int16_t direction;  /* Direction in which line increases in x. 1 or -1. */
    int16_t p;          /* (2 * dy) for octant 0, (2 * dx) for octant 1. */
    int16_t q;          /* (p - (2 * dx)) for octant 0, (p - (2 * dy)) for octant 1. */
    int16_t err;        /* Pixel Error term. (p - dx) for octant 0, (p - dy) for octant 1. */
} TriangleSideVarTracker;

/*
    Draws and fills triangles in one colour.

    First the procedure splits the triangle into a flat top and flat bottom triangle.
    These two-sub triangles are then filled separately.

    The process of filling a flat sub-triangle consists of running Bresenham's algorithm in parallel
    for two lines and drawing a horizontal line between them when the y coordinate is incremented.
    Further documentation on Bresenham's line algorithm and its implementation here is given among
    the drawLine function which was a precursor to the drawTriangle function. The drawLine function was
    adapted from an implementation by Michael Abrash.
*/
void drawTriangle(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], Triangle2D tri);