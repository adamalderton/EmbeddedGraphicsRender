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

    /*  */
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
        dx = -dx;

        if (dx > dy) {
            drawLineOctant0(frame, x0, y0, (uint16_t) dx, (uint16_t) dy, rgb, -1);
        } else {
            drawLineOctant1(frame, x0, y0, (uint16_t) dx, (uint16_t) dy, rgb, -1);
        }
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

    uint16_t rgb[FRAME_NUM_COLOURS] = {1, 0, 0};

    uint16_t point_0[2] = {3, 3};
    uint16_t point_1[2] = {3, 35};

    drawLine(frame, point_0, point_1, rgb);

    saveFrame(frame);

    return 0;
}