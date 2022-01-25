#include <stdint.h>

#include "graphics.h"

void drawPixel(
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t x,
    uint8_t y,
    uint8_t colour,
    uint8_t relative_intensity
)
{
    uint8_t shift = BITS_PER_PIXEL * (x % PIXELS_PER_BYTE);

    /*
        Write colour and intensity to pixel in one operation to pixel.
        To ensure pixels are overwritten correctly, we have to set the correct byte to 0 first.
    */

    frame[FRAME_NUM_ROWS - y - 1][x / PIXELS_PER_BYTE]
        +=
        ( -1 * (frame[FRAME_NUM_ROWS - y - 1][x / PIXELS_PER_BYTE] & (PIXEL_BITMASK << shift) ) )   /* Extract and remove current pixel value (usually 0). */
        + ( (colour << shift) + ( (relative_intensity << PIXELS_PER_BYTE) << shift ) );             /* Add new pixel value. */
}

uint8_t get_pixel_value_xy(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], uint8_t x, uint8_t y)
{
    uint8_t shift = BITS_PER_PIXEL * (x % PIXELS_PER_BYTE);

    return ( ( frame[FRAME_NUM_ROWS - y - 1][x / PIXELS_PER_BYTE] & (PIXEL_BITMASK << shift) ) >> shift );
}

uint8_t get_pixel_value_rowcol(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], uint8_t row, uint8_t col)
{
    uint8_t shift = BITS_PER_PIXEL * (col % PIXELS_PER_BYTE);

    return ( ( frame[row][col / PIXELS_PER_BYTE] & (PIXEL_BITMASK << shift) ) >> shift );
}

float dot_product_float_3d(float vec1[3], float vec2[3])
{
    return (vec1[X]*vec2[X]) + (vec1[Y]*vec2[Y]) + (vec1[Z]*vec2[Z]);
}

void cross_product_float_3d(float vec1[3], float vec2[3], float result[3])
{
    result[X] = (vec1[Y] * vec2[Z]) - (vec1[Z] * vec2[Y]);
    result[Y] = (vec1[Z] * vec2[X]) - (vec1[X] * vec2[Z]);
    result[Z] = (vec1[X] * vec2[Y]) - (vec1[Y] * vec2[X]);
}

