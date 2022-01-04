#include <stdint.h>

#include "draw_line.h"

/*
    Adapted from Graphics Programming Black Book - Michael Abrash.
    See drawLine() for further documentation.
*/
static void drawLineOctant1(
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t x0,
    uint8_t y0,
    uint8_t dx,
    uint8_t dy,
    uint8_t colour,
    uint8_t distance,
    int16_t x_direction /* Storing as int16_t prevents repeated casting. */
)
{
    int16_t dxx2;           /* dx \times 2 */
    int16_t dxx2minusdyx2;  /* (dx \times 2) - (dy \times 2) */
    int16_t err;            /* Pixel error term. */

    dxx2 = 2 * dx;
    dxx2minusdyx2 = dxx2 - (int16_t) (2 * dy);
    err = dxx2 - (int16_t) dy;

    drawPixel(frame, x0, y0, colour, MAX_RELATIVE_INTENSITY); /* Draw first pixel. */
    while ( dy-- > 0 ) {
        
        /* Test advancement of the x coordinate. */
        if (err >= 0) {
            x0 += x_direction;
            err += dxx2minusdyx2;
        } else {
            err += dxx2;
        }

        y0++;
        drawPixel(frame, x0, y0, colour, MAX_RELATIVE_INTENSITY);
    }
}

/*
    Adapted from Graphics Programming Black Book - Michael Abrash.
    See drawLine() for further documentation.
*/
static void drawLineOctant0(
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t x0,
    uint8_t y0,
    uint8_t dx,
    uint8_t dy,
    uint8_t colour,
    uint8_t distance,
    int16_t x_direction /* Storing as int16_t prevents repeated casting. */
)
{
    int16_t dyx2;           /* dy \times 2 */
    int16_t dyx2minusdxx2;  /* (dy \times 2) - (dx \times 2) */
    int16_t err;            /* Pixel error term. */

    dyx2 = 2 * dy;
    dyx2minusdxx2 = dyx2 - (int16_t) (2 * dx);
    err = dyx2 - (int16_t) dx;

    drawPixel(frame, x0, y0, colour, MAX_RELATIVE_INTENSITY); /* Draw first pixel. */
    while ( dx-- > 0 ) {

        /* Test advancement of the y coordinate. */
        if (err >= 0) {
            y0++;
            err += dyx2minusdxx2;

        } else {
            err += dyx2;
        }

        x0 += x_direction;
        drawPixel(frame, x0, y0, colour, MAX_RELATIVE_INTENSITY);
    }
}


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
    uint8_t distance
)
{
    uint8_t temp;
    uint8_t x0;
    uint8_t y0;
    uint8_t x1;
    uint8_t y1;
    int16_t dx;
    int16_t dy;

    x0 = point_0[X];
    y0 = point_0[Y];
    x1 = point_1[X];
    y1 = point_1[Y];

    /*
        Ensure that dy is always greater than 0.
        That way, only half the octants need be considered.
        That is, only line slopes between 0 and \pi as opposed to between
        0 and 2\pi.
    */
    if (y0 > y1) {
        temp = y0;
        y0 = y1;
        y1 = temp;

        temp = x0;
        x0 = x1;
        x1 = temp;
    }

    dx = x1 - x0;
    dy = y1 - y0;

    if (dx > 0) {
        if (dx > dy) {
            drawLineOctant0(frame, x0, y0, (uint8_t) dx, (uint8_t) dy, colour, distance, 1);
        } else {
            drawLineOctant1(frame, x0, y0, (uint8_t) dx, (uint8_t) dy, colour, distance, 1);
        }
    } else {
        dx = -dx; /* Ensures that line is only in octant 1 or 0. */

        if (dx > dy) {
            drawLineOctant0(frame, x0, y0, (uint8_t) dx, (uint8_t) dy, colour, distance, -1);
        } else {
            drawLineOctant1(frame, x0, y0, (uint8_t) dx, (uint8_t) dy, colour, distance, -1);
        }
    }
}

void drawHorizontalLine(
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t y,
    uint8_t x0,
    uint8_t x1,
    uint8_t colour,
    uint8_t distance
)
{
    for (uint8_t x = x0; x <= x1; x++) {
        drawPixel(frame, x, y, colour, distance);
    }
}