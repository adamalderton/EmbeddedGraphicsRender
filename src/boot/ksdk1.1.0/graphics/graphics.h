#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

/*=================== START OF DEMO SELECTION ======================*/

/*
    The demos available.
    Please only define one of these to be 1, the rest should be 0.
    They are given in increasing complexity as you go down the list.
*/

#define SPINNING_MULTICOLOUR_CUBE_DEMO 1

/*=================== END OF DEMO SELECTION ========================*/

/*================== START OF CONFIGURABLE QUANTITIES ==================*/

/*
    Must ensure that ((FRAME_NUM_ROWS * FRAME_NUM_COLS) / PIXELS_PER_BYTE) fits into the stack size
    with room left over for other variables, especially the graphic scene to be loaded. 
    FRAME_NUM_ROWS and FRAME_NUM_COLS must both be EVEN. Warnings are provided if not.

    GRAPHICS_OPTIMISED determines whether unneccessary .data and .bss variables elsewhere in the Warp
    firmware are used in the interest of memory. 1 implies optimised, 0 implies not optimised.
    Optimisation prevents printing as the print buffer is no longer used.
*/

#if (SPINNING_MULTICOLOUR_CUBE_DEMO)
    #define FRAME_NUM_ROWS 36
    #define FRAME_NUM_COLS 36
    #define L 0.55 /* Cube side length. Short variable name for later clarity. */
    #define NUM_TRIANGLES 12
    #define GRAPHICS_OPTIMISED 0
    #define ROTATION_RATE_THETA 3 /* Must be integer. */
    #define ROTATION_RATE_PHI 7   /* Must be integer. */
#endif

/* Used to display wireframe triangles - useful for debugging. 1 for yes, 0 for no. */
#define WIREFRAME 0

/* Used to display a square outline to display the limits of the frame on the OLED display. 1 for yes, 0 for no. */
#define OUTER_FRAME 0

/*
    Used to set the refresh rate of the display. See the 'FR Synchronisation' section of the SSD1331 manual.
    Should be between b0000 and b1111 which results in a divisor equal to the decimal value plus 1.
    That is, it ranges from 1 to 16. Set to 1 for highest refresh rate. This is often way too high for our purposes.
    There is very probably an analytic way to find out what this quantity should be for different renders, but in the
    interest if simplicity and focussing on the task at hand, this quantity should be found empirically.

    On further experimentation, it seems that should almost always be 0. If it's significant, the display noticeably
    flashes. That is, it's not bright for long enough on each clock cycle to appear continuous.
*/
#define REFRESH_RATE_DIVISOR 0

/*=================== END OF CONFIGURABLE QUANTITIES ===================*/

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
    2 pixels are stored in a byte, hence each is granted 4 bits of storage.
    2 bits per pixel are assigned to colour, hence the maximum 'colour' is b11 = 3.
    This means there are actually 4 colours (one being blank = off = black). The other two bits 
    are used to allow for different relative intensities. Similarly to colour, there are three options
    plus a fourth - 'off'.
*/

#define PIXELS_PER_BYTE 2
#define BITS_PER_PIXEL 4 /* sizeof(uint8_t) / PIXELS_PER_BYTE */

/* Number of colours available to be drawn. */
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

/*
    Tuneable thresholds at which to display certain pixel intensitities.
    These are compared with cos(theta) values derived from the dot product of a triangle
    and the light direction.
*/
#define RELATIVE_INTENSITY_0_THRESHOLD 0.0
#define RELATIVE_INTENSITY_1_THRESHOLD 0.5
#define RELATIVE_INTENSITY_2_THRESHOLD 0.8
#define RELATIVE_INTENSITY_3_THRESHOLD 1.0

/* Relative intensitites to be shown on screen. */
#define RELATIVE_INTENSITY_0 0
#define RELATIVE_INTENSITY_1 1
#define RELATIVE_INTENSITY_2 2
#define RELATIVE_INTENSITY_3 MAX_RELATIVE_INTENSITY


/*
    The amount by which to translate any vertices into the Z axis
    such that vertices are not rendered around (0, 0, 0).
    See documentation for z_translate in projection.c for much more infomation.
*/
#define Z_TRANSLATION 2.5

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

/* MEMSET not used as to avoid introduction of string.h. */
#define RESET_FRAME(frame) \
    for (uint8_t i = 0; i < FRAME_TRUE_ROWS; i++) { \
        for (uint8_t j = 0; j < FRAME_TRUE_COLS; j++) { \
            frame[i][j] = 0; \
        } \
    } \

typedef enum {
    X = 0,
    Y = 1,
    Z = 2
} Dims;

typedef enum {
    K = 0,  /* Black (Pixel OFF), 00 in binary. */
    R = 1,  /* Red, 01 in binary. */
    G = 2,  /* Green, 10 in binary. */
    B = 3   /* Blue, 11 in binary. */
} Colours;

/*
    While a 'vertex' struct would have had its advantages,
    a 2D array to represent the vertices was selected such that the
    vertices can be iteratively accessed - this circumvents the need
    for a lot of hardcoding for access.

    As a reminder, due to memory constraints, pixels cannot be composite colours.
    That is, shapes can only be red, green or blue. They can still be different shades of
    those colours in the interest of representing distance, for example.
*/
typedef struct {
    /* 
        Can only be one of those defined in the Colours enum.
        That is, only 00 for black, 01 for red, 10 for green or 11 for blue.
        Must fit in 2 bits.
    */
    uint8_t colour;

    /* Three three-dimensional vertices. */
    float vs[3][3];

    /* Normal vector to the triangle surface. Defined by the right-hand rule. */
    float normal[3];
} Triangle3D;

/*
    A type to store the scene to be rendered. Used as the triangles' normals do
    not need to be stored. It is simply the above but with the normal attribute
    removed.
*/
typedef struct {
    uint8_t colour;
    float vs[3][3];
} Triangle3DStorage;

/* The 2D version of the 3D triangle defined above. Has some extra attributes concerned with displaying. */
typedef struct {
    uint8_t colour;
    uint8_t relative_intensity; /* Must be 0, 1, 2, or 3. Must fit in 2 bits. */
    uint8_t vs[3][2];           /* Three two-dimensional vertices. */
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
void drawPixel(
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t x,
    uint8_t y,
    uint8_t colour,
    uint8_t relative_intensity
);

/* Gets the 4 bit pixel value by referencing the frame buffer in the x-y basis. */
uint8_t get_pixel_value_xy(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], uint8_t x, uint8_t y);

/* Gets the 4 bit pixel value by referencing the frame buffer in the row-column basis. */
uint8_t get_pixel_value_rowcol(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], uint8_t row, uint8_t col);

/* Returns simple dot product between two floating point vectors. */
float dot_product_float_3d(float vec1[3], float vec2[3]);

/* Calculates simple cross product between two floating point vectors, stores answer in result. */
void cross_product_float_3d(float vec1[3], float vec2[3], float result[3]);