#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

/*
    While a 'vertex' struct would have had its advantages,
    a 2D array to represent the vertices was selected such that the
    vertices can be iteratively accessed - this circumvents the need
    for a lot of hardcoding for access.

    As a reminder, due to memory constraints, pixels cannot be composite colours.
    That is, shapes can only be red, green or blue. They can still be different shades of
    those colours in the interest of representing distance, for example.
*/
typedef struct {
    /* 
        Can only be one of those defined in the Colours enum in graphics.h.
        That is, only 00 for black, 01 for red, 10 for green or 11 for blue.
        Must fit in 2 bits.
    */
    uint8_t colour;

    /*
        Must fit in 2 bits, hence be either 0, 1, 2, or 3.
    */
    uint8_t relative_intensity;

    /* Three two-dimensional vertices. */
    uint8_t vs[3][2];
} Triangle2D;

typedef struct {
    int16_t octant;     /* Octant of line. Either 0 or 1. */
    int16_t direction;  /* Direction in which line increases in x. 1 or -1. */
    int16_t p;          /* (2 * dy) for octant 0, (2 * dx) for octant 1. */
    int16_t q;          /* (p - (2 * dx)) for octant 0, (p - (2 * dy)) for octant 1. */
    int16_t err;        /* Pixel Error term. (p - dx) for octant 0, (p - dy) for octant 1. */
} TriangleSideVarTracker;

void drawTriangle(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], Triangle2D tri);