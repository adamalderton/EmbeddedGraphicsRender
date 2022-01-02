#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

/*
    While a 'vertex' struct would have had its advantages,
    a 2D array to represent the vertices was selected such that the
    vertices can be iteratively accessed - this circumvents the need
    for a lot of hardcoding for access.
*/
typedef struct {
    uint16_t rgb[FRAME_NUM_COLOURS];
    /* Three two-dimensional vertices. */
    uint16_t vs[3][2];
} Triangle2D;

typedef struct {
    int16_t octant;     /* Octant of line. Either 0 or 1. */
    int16_t direction;  /* Direction in which line increases in x. 1 or -1. */
    int16_t p;          /* (2 * dy) for octant 0, (2 * dx) for octant 1. */
    int16_t q;          /* (p - (2 * dx)) for octant 0, (p - (2 * dy)) for octant 1. */
    int16_t err;        /* Pixel Error term. (p - dx) for octant 0, (p - dy) for octant 1. */
} TriangleSideVarTracker;

void drawTriangle(uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS], Triangle2D tri);