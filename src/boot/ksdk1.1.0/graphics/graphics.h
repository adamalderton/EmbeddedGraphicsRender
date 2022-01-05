#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

/*================== START OF CUSTOMISABLE QUANTITIES ==================*/

/*
    Must ensure that ((FRAME_NUM_ROWS * FRAME_NUM_COLS) / PIXELS_PER_BYTE) fits into the stack size
    with room left over for other variables, especially the graphic scene to be loaded. 
*/
#define FRAME_NUM_ROWS 36
#define FRAME_NUM_COLS 36

/* Used to display wireframe triangles - useful for debugging. 1 for yes, 0 for no. */
#define WIREFRAME 1

/* Used to display a square outline to display the limits of the frame on the OLED display. 1 for yes, 0 for no. */
#define OUTER_FRAME 0

/*=================== END OF CUSTOMISABLE QUANTITIES ===================*/

#define SCREEN_MAX_COLS 96
#define SCREEN_MAX_ROWS 64

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
    The distance quantity must fit within 2 bits.
    The minimum relative intensity is 0.
*/
#define MAX_RELATIVE_INTENSITY 3
#define MAX_RELATIVE_INTENSITY_FLOAT 3.0 /* To prevent repeated casting. */

/*
    2 pixels are stored in a byte. Each is granted 4 bits of storage.
    2 bits per pixel are assigned to colour, hence the maximum 'colour' is b11 = 3.
    This means there are actually 4 colours (one being blank = off = black), however
    FRAME_NUM_COLOURS is defined as three for more pragmatic iteration in other parts 
    of the code.
*/
#define PIXELS_PER_BYTE 2
#define FRAME_NUM_COLOURS 3

/*
    Used to extract colour and distance out of 4 bit pixel value.

    Colour is stored in the rightmost bytes hence the bitmask is 0011.
    Distance is stored in the other two hence 1100.
*/
#define COLOUR_BITMASK 3
#define RELATIVE_INTENSITY_BITMASK 15 /* (COLOUR_BITMASK << PIXELS_PER_BYTE) */

/*
    00001111, can be left shifted to write to other pixel in byte.
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

#define WRITE_RELATIVE_INTENSITY_TO_PIXEL(frame, row, col, relative_intensity) \
    frame[row][col / PIXELS_PER_BYTE] += ( (relative_intensity << PIXELS_PER_BYTE) << (BITS_PER_PIXEL * (col % PIXELS_PER_BYTE)) );

#define RELATIVE_INTENSITY_FROM_PIXEL_VALUE(pixel_value) \
    ( (pixel_value & RELATIVE_INTENSITY_BITMASK) >> PIXELS_PER_BYTE )

#define COLOUR_FROM_PIXEL_VALUE(pixel_value) \
    ( (pixel_value & COLOUR_BITMASK) )

#define COPY_2D_VERTEX(dest, src) \
    dest[X] = src[X]; \
    dest[Y] = src[Y];

#define RESET_FRAME(frame) \
    for (uint8_t i = 0; i < FRAME_TRUE_ROWS; i++) { \
        for (uint8_t j = 0; j < FRAME_TRUE_COLS; j++) { \
            frame[i][j] = 0; \
        } \
    } \


	// for (uint8_t i = 0; i < FRAME_TRUE_ROWS; i++) { 
	// 	for (uint8_t j = 0; j < FRAME_TRUE_COLS; j++){ 
	// 		frame[i][j] = 0;
	// 	} 
	// }

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

void drawPixel(
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t x,
    uint8_t y,
    uint8_t colour,
    uint8_t relative_intensity
);

// uint8_t get_pixel_value_xy(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], uint8_t x, uint8_t y);
uint8_t get_pixel_value_rowcol(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], uint8_t row, uint8_t col);