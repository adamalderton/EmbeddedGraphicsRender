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

typedef struct {
    uint16_t x0;
    uint16_t y0;
    int16_t dx;
    int16_t dy;
    int16_t dxx2;
    int16_t dyx2;
    int16_t dxx2minusdyx2;
    int16_t dyx2minusdxx2;
    int16_t err0;
    int16_t err1;
    int16_t octant;
    int16_t direction;
} TriangleSideVarTracker;

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
    Sorts triangle vertices in descending y.
*/
static void sortTriangle2DVertices(Triangle2D *tri)
{
    if (tri->vs[0][Y] < tri->vs[2][Y]) {
        swap2DVertices(tri->vs[0], tri->vs[2]);
    }

    if (tri->vs[0][Y] < tri->vs[1][Y]) {
        swap2DVertices(tri->vs[0], tri->vs[1]);
    }

    if (tri->vs[1][Y] < tri->vs[2][Y]) {
        swap2DVertices(tri->vs[1], tri->vs[2]);
    }
}


static void drawHorizontalLine(
    uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS],
    uint16_t y,
    uint16_t x0,
    uint16_t x1,
    uint16_t rgb[3]
)
{
    for (uint16_t x = x0; x <= x1; x++) {
        drawPixel(frame, x, y, rgb);
    }
}

static TriangleSideVarTracker setupLineTracker(uint16_t point_0[2], uint16_t point_1[2])
{
    TriangleSideVarTracker tr;

    tr.x0 = point_0[X];
    tr.y0 = point_0[Y];

    tr.dx = point_1[X] - tr.x0;
    tr.dy = point_1[Y] - tr.y0;

    if (tr.dx > 0) {
        tr.direction = 1;
        if (tr.dx > tr.dy) {
            tr.octant = 0;
        } else {
            tr.octant = 1;
        }
    } else {
        tr.dx = -tr.dx;
        tr.direction = -1;
        if (tr.dx > tr.dy) {
            tr.octant = 0;
        } else {
            tr.octant = 1;
        }
    }

    tr.dxx2 = 2 * tr.dx;
    tr.dyx2 = 2 * tr.dy;
    tr.dxx2minusdyx2 = tr.dxx2 - tr.dyx2;
    tr.dyx2minusdxx2 = tr.dyx2 - tr.dxx2;
    tr.err0 = tr.dyx2 - tr.dx;
    tr.err1 = tr.dxx2 - tr.dy;

    return tr;
}

static void drawFlatTopTriangle(uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS], Triangle2D tri)
{
    /*
        As the vertices have been sorted, the flat line lies between vertex 0 and 1.
        Therefore, we need to draw a line from vertex 2 to vertex 1 (left) and 
        from vertex 2 to vertex 0 (right).

        We'll call the left line A and the right line B.
    */

    TriangleSideVarTracker A; /* Tracks progression along line A (left). */
    TriangleSideVarTracker B; /* Tracks progression along line B (right). */

    uint16_t y;         /* Tracks y coordinate. */
    uint16_t y_end;     /* y coordinate of top line. */
    uint16_t xA;        /* Leftmost x coordinate of left line (A) for a given y.. */
    uint16_t xB;        /* Rightmost y coordinate of right line (B) for a given y. */

    /*
        Ensure triangle follows right hand rule (normal out of the plane).
        This ensures that the 'right' line is on the 'right' and the 'left' line is on the left.
    */
    if (tri.vs[0][X] < tri.vs[1][X]) {
        swap2DVertices(tri.vs[0], tri.vs[1]);
    }

    A = setupLineTracker(tri.vs[2], tri.vs[1]);
    B = setupLineTracker(tri.vs[2], tri.vs[0]);

    /* If a flat top triangle is correctly passed, then tri.vs[0][Y] should equal tri.vs[1][Y]. */
    y = tri.vs[2][Y];
    y_end = tri.vs[1][Y];
    xA = tri.vs[2][X];
    xB = tri.vs[2][X];

    /* Draw first (bottom) point. */
    drawPixel(frame, tri.vs[2][X], y, tri.rgb);

    /*
        Flat top triangle rasterisation loop.

        Much of the duplicate code here could be put into a separate function however
        this was decided against as to prevent unnecessary numerous pointer dereference.
        A pointer to an octant specific function was also considered but decided against.

        It is a shame that if (A.octant == 0) has be carried out for A and B at each y, but
        in terms of code simplification, it was a price worth paying.
    */
    while (y < y_end) {

        y++;

        /*
            Update A and get xA.
        */
        if (A.octant == 0) {

            while (1) {
                xA += A.direction;

                if (A.err0 >= 0) {
                    A.err0 += A.dyx2minusdxx2;
                    break;
                } else {
                    A.err0 += A.dyx2;
                }
            }

        } else {

            if (A.err1 >= 0) {
                xA += A.direction;
                A.err1 += A.dxx2minusdyx2;
            } else {
                A.err1 += A.dxx2;
            }

        }

        /*
            Update B and get xB.
        */
        if (B.octant == 0) {

            while (1) {
                xB += B.direction;

                if (B.err0 >= 0) {
                    B.err0 += B.dyx2minusdxx2;
                    break;
                } else {
                    B.err0 += B.dyx2;
                }
            }

        } else {

            if (B.err1 >= 0) {
                xB += B.direction;
                B.err1 += B.dxx2minusdyx2;
            } else {
                B.err1 += B.dxx2;
            }

        }

        /* Finally, draw the line at y = y between xA and xB. */
        drawHorizontalLine(frame, y, xA, xB, tri.rgb);
    }

    printf("%d\n", xB);

}

void drawTriangle(uint16_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS], Triangle2D tri)
{
    /* Sort triangle vertices in ascending order. Triangle will be drawn from the bottom up. */
    sortTriangle2DVertices(&tri);

    /* Simple case of top flat triangle only. */
    if (tri.vs[0][Y] == tri.vs[1][Y]) {
        printf("Drawing flat top triangle...\n");
        drawFlatTopTriangle(frame, tri);
    } else {
        printf("What??\n");
    }

    /* Simple case of bottom flat triangle only. */

    /* General Case. */
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

    tri.vs[0][X] = 0; tri.vs[0][Y] = 10;
    tri.vs[1][X] = 20; tri.vs[1][Y] = 10;
    tri.vs[2][X] = 10; tri.vs[2][Y] = 0;

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