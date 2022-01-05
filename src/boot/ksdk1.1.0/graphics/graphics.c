#include <stdint.h>

#include "graphics.h"

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
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t x,
    uint8_t y,
    uint8_t colour,
    uint8_t relative_intensity
)
{
    uint8_t shift = BITS_PER_PIXEL * (x % PIXELS_PER_BYTE);

    /* Write colour and intensity to pixel in one operation to pixel. */
    frame[FRAME_NUM_ROWS - y - 1][x / PIXELS_PER_BYTE] += (colour << shift) + ( (relative_intensity << PIXELS_PER_BYTE) << shift );
}

// /* Gets the 4 bit pixel value by referencing the frame buffer in the x-y basis. */
// uint8_t get_pixel_value_xy(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], uint8_t x, uint8_t y)
// {
//     uint8_t shift = BITS_PER_PIXEL * (x % PIXELS_PER_BYTE);

//     return ( ( frame[y][x / PIXELS_PER_BYTE] & (PIXEL_BITMASK << shift) ) >> shift );
// }

/* Gets the 4 bit pixel value by referencing the frame buffer in the row-column basis. */
uint8_t get_pixel_value_rowcol(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], uint8_t row, uint8_t col)
{
    uint8_t shift = BITS_PER_PIXEL * (col % PIXELS_PER_BYTE);

    return ( ( frame[FRAME_NUM_ROWS - row - 1][col / PIXELS_PER_BYTE] & (PIXEL_BITMASK << shift) ) >> shift );
}
