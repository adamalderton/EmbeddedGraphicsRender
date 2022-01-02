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