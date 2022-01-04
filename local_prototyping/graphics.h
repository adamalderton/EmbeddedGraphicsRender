#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

#define SCREEN_MAX_COLS 96
#define SCREEN_MAX_ROWS 64

/*
    Under the 4 bit pixel description described elsewhere, there is room for a 56 x 56
    display window which uses nearly all of the vertical space in the screen. This frame buffer fits into
    1600 bytes.
*/
#define FRAME_NUM_ROWS 6
#define FRAME_NUM_COLS 6
#define PIXELS_PER_BYTE 2
#define FRAME_NUM_COLOURS 3

/*
    Used to easily defined the correct sized array.
*/
#define FRAME_TRUE_ROWS FRAME_NUM_ROWS
#define FRAME_TRUE_COLS (FRAME_NUM_COLS / PIXELS_PER_BYTE)

/*
    b11110.
    
    The SSD1331 has 5 bits for each colour (6 for green but that's truncated in this software)
    Therefore, the intensity sent to the display needs to be normalised to be between 0 and 31. (b00000 -> b11111)
    For ergonomic reasons, it makes sense to set the maximum intensity to a round 30, b11110.
*/
#define MAX_COLOUR_INTENSITY 30
#define MAX_COLOUR_INTENSITY_FLOAT 30.0

/*
    THe distance quantity must fit within 2 bits.
*/
#define MINIMUM_RELATIVE_INTENSITY 0
#define MAXIMUM_RELATIVE_INTENSITY 3
#define MAXIMUM_RELATIVE_INTENSITY_FLOAT 3.0 /* To prevent repeated casting. */

/*
    Used to extract colour and distance out of 4 bit pixel value.

    Colour is stored in the rightmost bytes hence the bitmask is 0011.
    Distance is stored in the other two hence 1100.
*/
#define COLOUR_BITMASK 3
#define RELATIVE_INTENSITY_BITMASK 15 /* (COLOUR_BITMASK << PIXELS_PER_BYTE) */

/*
    00001111, can be left shifted to write to other pixel.
*/
#define PIXEL_BITMASK 15
#define BITS_PER_PIXEL 4 /* sizeof(uint8_t) / PIXELS_PER_BYTE */

/*
    Colour can be directly written as, in theory, shapes are rendered in the
    correct order. Additionally, intensities should be written to the frame element AFTER the colour is written.

    If col is even, colour is not left shifted at all and is written into the 0th pixel.
    If col is odd, colour is left shifted by 4 spaces such that colour is written into the 1st pixel.
*/
#define WRITE_COLOUR_TO_PIXEL(frame, row, col, colour) \
    frame[row][col / PIXELS_PER_BYTE] += ( colour << (BITS_PER_PIXEL * (col % PIXELS_PER_BYTE)) );

/*
    Remember, the left-most 2 bits represent colour, so the intensity is left shifted.
    Therefore, dist cannot be greater than 3.
*/
#define WRITE_RELATIVE_INTENSITY_TO_PIXEL(frame, row, col, relative_intensity) \
    frame[row][col / PIXELS_PER_BYTE] += ( (relative_intensity << PIXELS_PER_BYTE) << (BITS_PER_PIXEL * (col % PIXELS_PER_BYTE)) );

#define RELATIVE_INTENSITY_FROM_PIXEL_VALUE(pixel_value) \
    ( (pixel_value & RELATIVE_INTENSITY_BITMASK) >> PIXELS_PER_BYTE )

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
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t x,
    uint8_t y,
    uint8_t colour,
    uint8_t distance
);