#ifndef STDINT
	#include <stdint.h>
	#define STDINT
#endif

#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

void drawLine(
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t point_0[2],
    uint8_t point_1[2],
    uint8_t colour,
    uint8_t distance
);

void drawHorizontalLine(
    uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS],
    uint8_t y,
    uint8_t x0,
    uint8_t x1,
    uint8_t colour,
    uint8_t distance
);