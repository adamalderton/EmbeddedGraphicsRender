#define FRAME_NUM_ROWS 64
#define FRAME_NUM_COLS 96
#define FRAME_NUM_COLOURS 3

/*
    b11111.
    
    The SSD1331 has 5 bits for each colour (6 for green but that's truncated in this software)
    Therefore, the colours sent to the display need to normalised to be between 0 and 31. (b00000 -> b11111)
*/
#define MAX_COLOUR 31

#define WRITE_TO_FRAME(frame, row, col, rgb) \
    frame[row][col][R] = rgb[R]; \
    frame[row][col][G] = rgb[G]; \
    frame[row][col][B] = rgb[B];

#define GET_FRAME_VALUE(frame, x, y, channel) \
    ( frame[FRAME_NUM_ROWS - y - 1][x][channel] )

#define COPY_2D_VERTEX(dest, src) \
    dest[X] = src[X]; \
    dest[Y] = src[Y];

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
void drawPixel(
    uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS],
    uint8_t x,
    uint8_t y,
    uint8_t rgb[FRAME_NUM_COLOURS]
);