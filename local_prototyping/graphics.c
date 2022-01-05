#include <stdint.h>
#include <stdio.h>

#include "graphics.h"

typedef enum {
	RED_LEFT_SHIFT = 0,
	GREEN_LEFT_SHIFT = 6,
	BLUE_LEFT_SHIFT = 11,
} ColourBitShifts;

volatile uint8_t	in_buffer[2];
volatile uint8_t	payload_bytes[2];

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

static void saveFrame(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS])
{
    FILE *outfile;

    outfile = fopen("frame.txt", "w");

    for (int y = FRAME_TRUE_ROWS - 1; y >= 0; y--) {
        for (int x = 0; x < FRAME_TRUE_COLS; x++) {
            fprintf(outfile, "%d ", frame[FRAME_TRUE_ROWS - y - 1][x]);
        }
        fprintf(outfile, "\n");
    }

    fclose(outfile);
}

uint8_t get_pixel_value_xy(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], uint8_t x, uint8_t y)
{
    uint8_t shift = BITS_PER_PIXEL * (x % PIXELS_PER_BYTE);

    return ( ( frame[FRAME_NUM_ROWS - y - 1][x / PIXELS_PER_BYTE] & (PIXEL_BITMASK << shift) ) >> shift );
}

uint8_t get_pixel_value_rowcol(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS], uint8_t col, uint8_t row)
{
    uint8_t shift = BITS_PER_PIXEL * (col % PIXELS_PER_BYTE);

    return ( ( frame[row][col / PIXELS_PER_BYTE] & (PIXEL_BITMASK << shift) ) >> shift );
}

int main(void)
{
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS];

    for (int i = 0; i < FRAME_TRUE_ROWS; i++) {
        for (int j = 0; j < FRAME_TRUE_COLS; j++) {
            frame[i][j] = 0;
        }
    }

    drawPixel(frame, 0, 0, 3, 3);
    drawPixel(frame, 1, 0, 3, 3);
    
    drawPixel(frame, 2, 3, 1, 3);
    drawPixel(frame, 3, 3, 2, 3);

    saveFrame(frame);

	uint16_t payload;
	uint8_t pixel_value; /* Contains both colour and 'distance'. */

	/*
		Stores the amounts by which to left shift each colour such that it is displayed appropriately. This
		array is iterated over as necessary later (hence not an enum).
	*/
	uint8_t shifts[FRAME_NUM_COLOURS];

	shifts[0] = RED_LEFT_SHIFT;
	shifts[1] = GREEN_LEFT_SHIFT;
	shifts[2] = BLUE_LEFT_SHIFT;

	payload = 0;

    /* Get pixel value, extract colour and distance. */
    pixel_value = get_pixel_value_xy(frame, 0, 0);

    /* If both colour and intensity are non zero. Zero colour is black. */
    if ((pixel_value & COLOUR_BITMASK) && (pixel_value & RELATIVE_INTENSITY_BITMASK)) {

        /* Colour only first */
        printf("%d\n", pixel_value);

        float fraction = ((float) (RELATIVE_INTENSITY_FROM_PIXEL_VALUE(pixel_value))) / (MAX_RELATIVE_INTENSITY_FLOAT);
        uint8_t intensity = (uint8_t) (MAX_COLOUR_INTENSITY_FLOAT * fraction);

        // payload = ( ( MAX_COLOUR_INTENSITY * ( (uint8_t) ( (float) (pixel_value & RELATIVE_INTENSITY_BITMASK) / MAX_COLOUR_INTENSITY_FLOAT ) ) ) << shifts[(pixel_value & COLOUR_BITMASK) - 1] );

        printf("%f\n", fraction);
        printf("%d\n", intensity << shifts[(pixel_value & COLOUR_BITMASK) - 1]);
    }

}