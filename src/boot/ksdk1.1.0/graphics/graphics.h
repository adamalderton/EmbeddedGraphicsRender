#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

#define FRAME_NUM_ROWS 40
#define FRAME_NUM_COLS 40
#define FRAME_NUM_COLOURS 3

/*
    b11111.
    
    The SSD1331 has 5 bits for each colour (6 for green but that's truncated in this software)
    Therefore, the colours sent to the display need to normalised to be between 0 and 31. (b00000 -> b11111)
*/
#define MAX_COLOUR 31

/*
    Used for simple 2D demos. When this is passed to functions like drawPixel,
    it ensures the pixel drawn is as bright as possible.
*/
#define CLOSEST_DISTANCE 0

/*
    Colour can be directly written as, in theory, shapes are rendered in the
    correct order.

    Additionally, distances should be written to the frame element AFTER the colour is written.
*/
#define WRITE_COLOUR_TO_PIXEL(frame, row, col, colour) \
    frame[row][col] = colour;

/*
    Remember, the left-most 6 bits represent colour, so the distance is left shifted.
    Therefore, dist cannot be greater than 63. That is, 0 and 63 must represent the
    distance range.
*/
#define WRITE_DISTANCE_TO_PIXEL(frame, row, col, dist) \
    frame[row][col] += (dist << 2);

#define GET_PIXEL_VALUE(frame, x, y) \
    ( frame[FRAME_NUM_ROWS - y - 1][x] )

#define COPY_2D_VERTEX(dest, src) \
    dest[X] = src[X]; \
    dest[Y] = src[Y];

typedef enum {
    X,
    Y,
    Z
} Dims;

typedef enum {
    K = 0,  /* Black (Pixel OFF), 00 in binary. */
    R = 1,  /* Red, 01 in binary. */
    G = 2,  /* Green, 10 in binary. */
    B = 3   /* Blue, 11 in binary. */
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
void drawPixel(
    uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS],
    uint8_t x,
    uint8_t y,
    uint8_t colour,
    uint8_t distance,
);