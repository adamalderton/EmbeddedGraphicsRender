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

#define COPY_2D_VERTEX(dest, src) \
    dest[X] = src[X]; \
    dest[Y] = src[Y];

#define SIGNUM(x) \
    ((x > 0) ? 1 : ((x < 0) ? -1 : 0))

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
    uint16_t rgb[FRAME_NUM_COLOURS];
    /* Three two-dimensional vertices. */
    uint16_t vs[3][2];
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

static void swap2DVertices(uint16_t v0[2], uint16_t v1[2])
{
    uint16_t temp[2];

    COPY_2D_VERTEX(temp, v0);
    COPY_2D_VERTEX(v0, v1);
    COPY_2D_VERTEX(v1, temp);
}

/*
    Sorts triangle vertices in ascending y.
*/
static void sortTriangle2DVertices(Triangle2D *tri)
{
    if (tri->vs[0][Y] > tri->vs[2][Y]) {
        swap2DVertices(tri->vs[0], tri->vs[2]);
    }

    if (tri->vs[0][Y] > tri->vs[1][Y]) {
        swap2DVertices(tri->vs[0], tri->vs[1]);
    }

    if (tri->vs[1][Y] > tri->vs[2][Y]) {
        swap2DVertices(tri->vs[1], tri->vs[2]);
    }
}

static void drawHorizontalLine(
    uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS],
    uint16_t point_0[2],
    uint16_t point_1[2],
    uint16_t rgb[3]
)
{
    if (point_0[Y] != point_1[Y]) {
        printf("Line not horizontal!\n");
    }

    uint16_t y = point_0[Y];

    for (uint16_t x = point_0[X]; x < point_1[X]; x++) {
        drawPixel(frame, x, y, rgb);
    }
}

static void drawFlatSideTriangle(uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS], Triangle2D tri)
{
    uint16_t temp;
    uint16_t temp_vert1[2];
    uint16_t temp_vert2[2];

    int8_t swapped1 = 0;
    int8_t swapped2 = 0;

    uint16_t dx1 = abs(tri.vs[1][X] - tri.vs[0][X]);
    uint16_t dy1 = abs(tri.vs[1][Y] - tri.vs[0][Y]);
    
    uint16_t dx2 = abs(tri.vs[2][X] - tri.vs[0][X]);
    uint16_t dy2 = abs(tri.vs[2][Y] - tri.vs[0][Y]);

    int16_t x1_incre = SIGNUM(tri.vs[1][X] - tri.vs[0][X]);
    int16_t x2_incre = SIGNUM(tri.vs[2][X] - tri.vs[0][X]);
    int16_t y1_incre = SIGNUM(tri.vs[1][Y] - tri.vs[0][Y]);
    int16_t y2_incre = SIGNUM(tri.vs[2][Y] - tri.vs[0][Y]);

    /* Pixel error terms. */
    int16_t err1;
    int16_t err2;

    COPY_2D_VERTEX(temp_vert1, tri.vs[0]);
    COPY_2D_VERTEX(temp_vert2, tri.vs[0]);

    if (dy1 > dx1) {
        temp = dx1;
        dx1  = dy1;
        dy1 = temp;
        swapped1 = 1;
    }

    if (dy2 > dx2) {
        temp = dx2;
        dx2 = dy2;
        dy2 = temp;
        swapped2 = 1;
    }

    err1 = (2 * dy1) - dx1;
    err2 = (2 * dy2) - dx2;

    for (int16_t i = 0; i <= dx1; i++) {
        drawHorizontalLine(frame, temp_vert1, temp_vert2, tri.rgb);

        while (err1 >= 0) {
            if (swapped1) {
                temp_vert1[X] += x1_incre;
            } else {
                temp_vert1[Y] += y1_incre;
            }
            err1 -= 2 * dx1;
        }

        if (swapped1) {
            temp_vert1[Y] += y1_incre;
        } else {
            temp_vert1[X] += x1_incre;
        }
        err1 += 2 * dy1;

        /* Iterate on line 2 until it reaches line 1. */
        while (temp_vert2[Y] != temp_vert1[Y]) {
            while (err2 >= 0) {
                if (swapped2) {
                    temp_vert2[X] += x2_incre;
                } else {
                    temp_vert2[Y] += y2_incre;
                }
            }

            if (swapped2) {
                temp_vert2[Y] += y2_incre;
            } else {
                temp_vert2[X] += x2_incre;
            }

            err2 += 2 * dy2;
        }
    }
}

void drawTriangle(uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS], Triangle2D tri)
{
    /* Sort vertices in ascending y. */
    sortTriangle2DVertices(&tri);

    /* Check for bottom-flat triangle. */
    if (tri.vs[1][Y] == tri.vs[2][Y]) {
        drawFlatSideTriangle(frame, tri);
    }
    
    /* Check for top-flat triangle. */
    else if (tri.vs[0][Y] == tri.vs[1][Y]) {

        /* Cyclically shift the three vertices by one place. */
        swap2DVertices(tri.vs[0], tri.vs[2]);
        swap2DVertices(tri.vs[1], tri.vs[2]);

        drawFlatSideTriangle(frame, tri);
    }

    /* General case - a top-flat and a bottom-flat triangle. */
    else {
        uint16_t new_vertex[2];     /* New vertex to split the triangles. */
        uint16_t temp[2];           /* Stores temporary vertex. */

        new_vertex[X] = (uint16_t) (tri.vs[0][X] + ( (float) (tri.vs[1][Y] - tri.vs[0][Y]) / (float) (tri.vs[2][Y] - tri.vs[0][Y])) * (tri.vs[2][X] - tri.vs[0][X]));
        new_vertex[Y] = tri.vs[1][Y];

        /*
            Draw bottom flat triangle.
        */
        COPY_2D_VERTEX(temp, tri.vs[2]);
        COPY_2D_VERTEX(tri.vs[2], new_vertex);
        drawFlatSideTriangle(frame, tri);

        /*
            Draw top flat triangle.
        */
        COPY_2D_VERTEX(tri.vs[0], temp);
        drawFlatSideTriangle(frame, tri);
    }
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

    Triangle2D tri;

    tri.vs[0][X] = 10; tri.vs[0][Y] = 10;
    tri.vs[1][X] = 80; tri.vs[1][Y] = 20;
    tri.vs[2][X] = 50; tri.vs[2][Y] = 45;

    tri.rgb[R] = 1;
    tri.rgb[G] = 0;
    tri.rgb[B] = 0;

    // drawLine(frame, tri.vs[0], tri.vs[1], tri.rgb);
    // drawLine(frame, tri.vs[1], tri.vs[2], tri.rgb);
    // drawLine(frame, tri.vs[2], tri.vs[0], tri.rgb);

    drawTriangle(frame, tri);

    saveFrame(frame);

    return 0;
}