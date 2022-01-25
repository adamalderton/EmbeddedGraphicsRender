#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

/*
    Draws lines via the Bresenham algorithm.

    There are eight octants in the parameter space that defines 
    line drawing. That is, x increases faster than y, y increases
    faster than x, y is increasing faster than |x| is increasing as x grows
    negatively, etc. Explanation can be simplified by parameterising this slope
    in terms of its inclination angle \theta. The first octant corresponds to 
    0 <= \theta <= (\pi / 4), for example, such that there are eight octants.

    The 'lower half' of the octants, being \pi <= \theta < (2 * \pi) can be made equivalent to swapping
    the y values of the demarcating points. Additionally, the remaining octancs (0 - 3 with 0 indexing) can be halfed 
    once more by swapping the x values of the points as necessary, such that the line to be drawn
    has a slope bounded by 0 <= \theta <= (\pi / 2). Therefore, following this procedure,
    drawLine below selects which octant the line belongs to, 0 or 1, and swaps the points as necessary.
    It then calls the correct octant line drawing procedure.
*/
void drawLine(
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t point_0[2],
    uint8_t point_1[2],
    uint8_t colour,
    uint8_t relative_intensity
);

/* A special case of drawLine(). The Bresenham algorithm does not need to be used, only simple array indexing. */
void drawHorizontalLine(
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t y,
    uint8_t x0,
    uint8_t x1,
    uint8_t colour,
    uint8_t relative_intensity
);