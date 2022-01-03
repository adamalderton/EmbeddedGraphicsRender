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
    uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS],
    uint8_t x,
    uint8_t y,
    uint8_t colour,
    uint8_t distance
)
{
    uint8_t row = FRAME_NUM_ROWS - y - 1;
    uint8_t col = x;

    WRITE_COLOUR_TO_PIXEL(frame, row, col, colour);
    
    WRITE_DISTANCE_TO_PIXEL(frame, row, col, distance);
}