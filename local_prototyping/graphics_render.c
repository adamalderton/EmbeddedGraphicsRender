#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define FRAME_NUM_ROWS 64
#define FRAME_NUM_COLS 96
#define FRAME_NUM_COLOURS 3

#define WRITE_TO_FRAME(frame, row, col, rgb) \
    frame[row][col][R] = rgb[R]; \
    frame[row][col][G] = rgb[G]; \
    frame[row][col][B] = rgb[B];

#define GET_FRAME_VALUE(frame, x, y, channel) \
    ( frame[FRAME_NUM_ROWS - y - 1][x][channel] )

typedef enum {
    X,
    Y,
    Z
} Dims;

typedef enum {
    R,
    G,
    B
} Colours;

/*
    While a 'vertex' struct would have had its advantages,
    a 2D array to represent the vertices was selected such that the
    vertices can be iteratively accessed - this circumvents the need
    for a lot of hardcoding for access.
*/

typedef struct {
    uint16_t vertices[3][2];
} Triangle2D;

/*
    Defines the means to access the 'frame' array correctly.
    
    In C style indexing, the point (x = 0, y = 0) is at the top left of the frame.

    With increasing x, the point of consideration travels DOWN VERTICALLY.
    With increasing y, the point of consideration travels RIGHT HORIZONTALLY.

    This can be somewhat confusing with the intuition that a cartesian coordinate system
    would start in the bottom left such that x and y are only positive and increase horizontally
    right and vertically up respectively.

    Therefore, to convert from C array access to an inuitive coordinate system, the following macros
    are implemented.
 */
static void drawPixel(
    uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS],
    uint16_t x,
    uint16_t y,
    uint16_t rgb[FRAME_NUM_COLOURS]
)
{
    uint16_t row = FRAME_NUM_ROWS - y - 1;
    uint16_t col = x;

    WRITE_TO_FRAME(frame, row, col, rgb);
}

/*
    Adapted from Graphics Programming Black Book - Michael Abrash.
    See drawLine() for further documentation.
*/
static void drawLineOctant1(
    uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS],
    uint16_t x0,
    uint16_t y0,
    uint16_t dx,
    uint16_t dy,
    uint16_t rgb[FRAME_NUM_COLOURS],
    int16_t x_direction
)
{
    int16_t dxx2;           /* dx \times 2 */
    int16_t dxx2minusdyx2;  /* (dx \times 2) - (dy \times 2) */
    int16_t err;            /* Pixel error term. */

    dxx2 = 2 * dx;
    dxx2minusdyx2 = dxx2 - (int16_t) (2 * dy);
    err = dxx2 - (int16_t) dy;

    drawPixel(frame, x0, y0, rgb); /* Draw first pixel. */
    while ( dy-- > 0 ) {
        
        /* Test advancement of the x coordinate. */
        if (err >= 0) {
            x0 += x_direction;
            err += dxx2minusdyx2;
        } else {
            err += dxx2;
        }

        y0++;
        drawPixel(frame, x0, y0, rgb);
    }
}

/*
    Adapted from Graphics Programming Black Book - Michael Abrash.
    See drawLine() for further documentation.
*/
static void drawLineOctant0(
    uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS],
    uint16_t x0,
    uint16_t y0,
    uint16_t dx,
    uint16_t dy,
    uint16_t rgb[FRAME_NUM_COLOURS],
    int16_t x_direction
)
{
    int16_t dyx2;           /* dy \times 2 */
    int16_t dyx2minusdxx2;  /* (dy \times 2) - (dx \times 2) */
    int16_t err;            /* Pixel error term. */

    dyx2 = 2 * dy;
    dyx2minusdxx2 = dyx2 - (int16_t) (2 * dx);
    err = dyx2 - (int16_t) dx;

    drawPixel(frame, x0, y0, rgb); /* Draw first pixel. */
    while ( dx-- > 0 ) {

        /* Test advancement of the y coordinate. */
        if (err >= 0) {
            y0++;
            err += dyx2minusdxx2;

        } else {
            err += dyx2;
        }

        x0 += x_direction;
        drawPixel(frame, x0, y0, rgb);
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
    uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS],
    uint16_t point_0[2],
    uint16_t point_1[2],
    uint16_t rgb[FRAME_NUM_COLOURS]
)
{
    uint16_t temp;
    uint16_t x0;
    uint16_t y0;
    uint16_t x1;
    uint16_t y1;
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
            drawLineOctant0(frame, x0, y0, (uint16_t) dx, (uint16_t) dy, rgb, 1);
        } else {
            drawLineOctant1(frame, x0, y0, (uint16_t) dx, (uint16_t) dy, rgb, 1);
        }
    } else {
        dx = -dx; /* Ensures that line is only in octant 1 or 0. */

        if (dx > dy) {
            drawLineOctant0(frame, x0, y0, (uint16_t) dx, (uint16_t) dy, rgb, -1);
        } else {
            drawLineOctant1(frame, x0, y0, (uint16_t) dx, (uint16_t) dy, rgb, -1);
        }
    }

}

/*
    Sorts triangle vertices in ascending y.
*/
static void sortTriangleVertices(Triangle2D *tri)
{
    uint16_t temp[2];

    if (tri->vertices[0][Y] > tri->vertices[2][Y]) {
        temp[X] = tri->vertices[0][X];
        temp[Y] = tri->vertices[0][Y];

        tri->vertices[0][X] = tri->vertices[2][X];
        tri->vertices[0][Y] = tri->vertices[2][Y];

        tri->vertices[2][X] = temp[X];
        tri->vertices[2][Y] = temp[Y];
    }

    if (tri->vertices[0][Y] > tri->vertices[1][Y]) {
        temp[X] = tri->vertices[0][X];
        temp[Y] = tri->vertices[0][Y];

        tri->vertices[0][X] = tri->vertices[1][X];
        tri->vertices[0][Y] = tri->vertices[1][Y];

        tri->vertices[1][X] = temp[X];
        tri->vertices[1][Y] = temp[Y];
    }

    if (tri->vertices[1][Y] > tri->vertices[2][Y]) {
        temp[X] = tri->vertices[1][X];
        temp[Y] = tri->vertices[1][Y];

        tri->vertices[1][X] = tri->vertices[2][X];
        tri->vertices[1][Y] = tri->vertices[2][Y];

        tri->vertices[2][X] = temp[X];
        tri->vertices[2][Y] = temp[Y];
    }
}

static void drawFlatSideTriangle(Triangle2D tri, uint16_t rgb[FRAME_NUM_COLOURS])
{

}

void drawTriangle(Triangle2D tri, uint16_t rgb[FRAME_NUM_COLOURS])
{
    /* Sort vertices in ascending y. */


    /* Check for bottom-flat triangle. */

    /* Check for top-flat triangle. */

    /* General case - a top-flat and a bottom-flat triangle. */
}

static void saveFrame(uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS])
{
    FILE *outfile;

    outfile = fopen("frame.txt", "w");

    for (int y = FRAME_NUM_ROWS - 1; y >= 0; y--) {
        for (int x = 0; x < FRAME_NUM_COLS; x++) {
            fprintf(outfile, "%d ", GET_FRAME_VALUE(frame, x, y, R));
        }
        fprintf(outfile, "\n");
    }

    fclose(outfile);
}


int main(void)
{
    uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS] = {{{0}}};

    uint16_t rgb[FRAME_NUM_COLOURS] = {1, 0, 0};

    Triangle2D tri;

    tri.vertices[0][X] = 1;
    tri.vertices[0][Y] = 5;

    tri.vertices[1][X] = 2;
    tri.vertices[1][Y] = 6;

    tri.vertices[2][X] = 3;
    tri.vertices[2][Y] = 4;

    sortTriangleVertices(&tri);

    for (int i = 0; i < 3; i++) {
        printf("(%d, %d)\n", tri.vertices[i][X], tri.vertices[i][Y]);
    }

    return 0;
}