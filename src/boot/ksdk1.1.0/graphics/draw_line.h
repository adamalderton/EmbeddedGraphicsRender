#ifndef GRAPHICS
	#include "graphics.h"
	#define GRAPHICS
#endif

void drawLine(
    uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS],
    uint8_t point_0[2],
    uint8_t point_1[2],
    uint8_t rgb[FRAME_NUM_COLOURS]
);

void drawHorizontalLine(
    uint8_t frame[FRAME_NUM_ROWS][FRAME_NUM_COLS][FRAME_NUM_COLOURS],
    uint8_t y,
    uint8_t x0,
    uint8_t x1,
    uint8_t rgb[FRAME_NUM_COLOURS]
);