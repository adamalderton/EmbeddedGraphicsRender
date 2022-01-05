#include <stdint.h>

#include "fsl_spi_master_driver.h"
#include "fsl_port_hal.h"

#include "SEGGER_RTT.h"
#include "gpio_pins.h"
#include "warp.h"
#include "devSSD1331.h"

volatile uint8_t	in_buffer[2];
volatile uint8_t	payload_bytes[2];

/*
 *	Override Warp firmware's use of these pins and define new aliases.
 */
enum
{
	kSSD1331PinMOSI		= GPIO_MAKE_PIN(HW_GPIOA, 8),
	kSSD1331PinSCK		= GPIO_MAKE_PIN(HW_GPIOA, 9),
	kSSD1331PinCSn		= GPIO_MAKE_PIN(HW_GPIOB, 11),
	kSSD1331PinDC		= GPIO_MAKE_PIN(HW_GPIOA, 12),
	kSSD1331PinRST		= GPIO_MAKE_PIN(HW_GPIOB, 0),
};

static int writeCommand(uint8_t commandByte)
{
	spi_status_t status;

	/*
	 *	Drive /CS low.
	 *
	 *	Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(10);
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);

	payload_bytes[0] = commandByte;
	status = SPI_DRV_MasterTransferBlocking(
					0,			/* Master instance. */
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict) &payload_bytes[0],
					(uint8_t * restrict) &in_buffer[0],
					1			/* Transfer size */,
					1000		/* Timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}

/*
	With a frame fully drawn in the 'frame' array, we now write it to the Graphics Display RAM
	(GDRAM) within the chip over an SPI interface. Please see the SSD1331 datasheet for more information.

	It is wise to consider FR synchronisation with the writing process of the SSD1331 to the display. Implemented
	properly, a 'shearing' effect is minimised which would otherwise be a problem as the GDRAM is written to
	asynchronously.
*/
void writeFrame(uint8_t frame[FRAME_TRUE_ROWS][FRAME_TRUE_COLS])
// void writeFrame(void)
{
	/*
		The default writing format in which the SSD1331 chip expects to receive data is column wise. That is,
		each time a data write is performed, the chip internally increments its column pointer such that the
		next data byte is written into the sequestial GDRAM address. Fortunately, that agrees well with C style
		indexing, so we leverage this below.

		As a reminder, the GDRAM expects to be written to from top left to bottom right. However, our 'frame' coordinate system
		extends from the bottom left to the rop right. Therefore, we need to iterate over the frame appropriately (C style indexing).

		Each pixel is represented by 16 bits in GDRAM which represents the intensity of each colour. In the default setting,
		red has 5 bits, green has 5 bits and blue has 5 bits. The organisation of the bit (and the datastream) is as follows:

		B_4, B_3, B_2, B_1, B_0, G_5, G_4, G_3, G_2, G_1, G_0, R_4, R_3, R_2, R_1, R_0

		Bitmasking is used below to extract and compute the final colour from the 8 bits representing each pixel.
	*/

	uint16_t payload;
	uint8_t pixel_value; /* Contains both colour and 'distance'. */

	/*
		Stores the amounts by which to left shift each colour such that it is displayed appropriately. This
		array is iterated over as necessary later (hence not an enum). FRAME_NUM_COLOURS + 1 is used to account for black (no colour).
	*/
	uint8_t shifts[FRAME_NUM_COLOURS + 1];

	shifts[K] = 0;
	shifts[R] = RED_LEFT_SHIFT;
	shifts[G] = GREEN_LEFT_SHIFT;
	shifts[B] = BLUE_LEFT_SHIFT;

	/* Drive CS low. */
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/* Drive DC high. This ensures that the SSD1331 is expecting DATA as opposed to a command. */
	GPIO_DRV_SetPinOutput(kSSD1331PinDC);

	for (uint8_t row = FRAME_NUM_ROWS - 1; row != 255; row--) { /* row != 255 as row is unsigned. */
		for (uint8_t col = 0; col < FRAME_NUM_COLS; col++) {
			payload = 0;

			pixel_value = get_pixel_value_rowcol(frame, col, row);

			warpPrint("%d ", pixel_value);

			/*
				Calculate final result to store in the 16 bit payload. This is simply the colour and the ratio of
				the distance to the maximum distance mapped to the minimum and maximum intensity.
				Left shift the result as appropriate such that the correct colour is displayed.

				If both colour and intensity are non zero. Zero colour is black (pixel off).
			*/
			if ((pixel_value & COLOUR_BITMASK) && (pixel_value & RELATIVE_INTENSITY_BITMASK)) {
				payload = ( (uint8_t) (MAX_COLOUR_INTENSITY_FLOAT * (((float) (RELATIVE_INTENSITY_FROM_PIXEL_VALUE(pixel_value))) / (MAX_RELATIVE_INTENSITY_FLOAT))) ) << shifts[COLOUR_FROM_PIXEL_VALUE(pixel_value)];
			}

			/* Split 16 bit representation into two bytes to store in payload_bytes. */
			payload_bytes[0] = (0xFF00 & payload) >> 8; 	/* MSB. */
			payload_bytes[1] = (0xFF & payload);			/* LSB. */

			SPI_DRV_MasterTransferBlocking(
				0,			/* Master instance. */
				NULL		/* spi_master_user_config_t */,
				(const uint8_t * restrict) &payload_bytes[0],
				(uint8_t * restrict) &in_buffer[0],
				2			/* Transfer size in bytes */,
				1000		/* Timeout in microseconds (unlike I2C which is ms) */);

			/* Column pointer in SSD1331 internally updates here. */
		}
		/* Row column pointer in SSD1331 internally updates here. */
		warpPrint("\n");
	}

	/* Upon the final data read, the SSD1331 resets the internal row and column pointers. */

	/* Drive CS high to complete frame writing interaction. */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
}

void devSSD1331init(void)
{
		/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Re-configure SPI to be on PTA8 and PTA9 for MOSI and SCK respectively.
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 8u, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 9u, kPortMuxAlt3);

	warpEnableSPIpins();

	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Reconfigure to use as GPIO.
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 11u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTA_BASE, 12u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTB_BASE, 0u, kPortMuxAsGpio);


	/*
	 *	RST high->low->high.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_ClearPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);

	/*
	 *	Initialization sequence, borrowed from https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino
	 */
	writeCommand(kSSD1331CommandDISPLAYOFF);	// 0xAE
	writeCommand(kSSD1331CommandSETREMAP);		// 0xA0
	writeCommand(0x72);							// RGB Color
	writeCommand(kSSD1331CommandSTARTLINE);		// 0xA1
	writeCommand(0x0);
	writeCommand(kSSD1331CommandDISPLAYOFFSET);	// 0xA2
	writeCommand(0x0);
	writeCommand(kSSD1331CommandNORMALDISPLAY);	// 0xA4
	writeCommand(kSSD1331CommandSETMULTIPLEX);	// 0xA8
	writeCommand(0x3F);							// 0x3F 1/64 duty
	writeCommand(kSSD1331CommandSETMASTER);		// 0xAD
	writeCommand(0x8E);
	writeCommand(kSSD1331CommandPOWERMODE);		// 0xB0
	writeCommand(0x0B);
	writeCommand(kSSD1331CommandPRECHARGE);		// 0xB1
	writeCommand(0x31);
	writeCommand(kSSD1331CommandCLOCKDIV);		// 0xB3
	writeCommand(0xF0);							// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8A
	writeCommand(0x64);
	writeCommand(kSSD1331CommandPRECHARGEB);	// 0x8B
	writeCommand(0x78);
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8C
	writeCommand(0x64);
	writeCommand(kSSD1331CommandPRECHARGELEVEL);	// 0xBB
	writeCommand(0x3A);
	writeCommand(kSSD1331CommandVCOMH);		// 0xBE
	writeCommand(0x3E);
	writeCommand(kSSD1331CommandMASTERCURRENT);	// 0x87
	writeCommand(0x06);
	writeCommand(kSSD1331CommandCONTRASTA);		// 0x81
	writeCommand(0x91);
	writeCommand(kSSD1331CommandCONTRASTB);		// 0x82
	writeCommand(0x50);
	writeCommand(kSSD1331CommandCONTRASTC);		// 0x83
	writeCommand(0x7D);
	writeCommand(kSSD1331CommandDISPLAYON);		// Turn on OLED panel

	SEGGER_RTT_WriteString(0, "\r\n\tDone with initialization sequence...\n");

	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);
	SEGGER_RTT_WriteString(0, "\r\n\tDone with screen clear...\n");

	SEGGER_RTT_WriteString(0, "\r\n\tShow a sample rectangle to confirm successfull interaction with display\n");

	/*
		Draw a blue rectangle with a green border to test the screen.
	*/
	writeCommand(kSSD1331CommandFILL);
	writeCommand(0x01);

	writeCommand(kSSD1331CommandDRAWRECT);

	/* Screen has 95 columns and 63 rows, hence whole screen is specified below. */
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);

	/* Set outline colour (white). */
	writeCommand(0xFF);
	writeCommand(0xFF);
	writeCommand(0xFF);

	/* Set rectangle fill colour (brightest blue). */
	writeCommand(0);
	writeCommand(0);
	writeCommand(0xFF);

	/* Show rectangle for 1 second. */
	OSA_TimeDelay(1000);

	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);
	SEGGER_RTT_WriteString(0, "\r\n\tDone with sample rectangle.\n");

	/*
		The end of the standard initialisation sequence.

		Next, we set the write area of the screen. This assumes that the constants defined below
		are even, although truncation is likely okay if not.
	*/
	writeCommand(kSSD1331CommandSETCOLUMN);
	writeCommand((SCREEN_MAX_COLS / 2) - (FRAME_NUM_COLS / 2)); /* Starting column. */
	writeCommand((SCREEN_MAX_COLS / 2) + (FRAME_NUM_COLS / 2) - 1); /* End column. */
	
	writeCommand(kSSD1331CommandSETROW);
	writeCommand((SCREEN_MAX_ROWS / 2) - (FRAME_NUM_ROWS / 2)); /* Start row. */
	writeCommand((SCREEN_MAX_ROWS / 2) + (FRAME_NUM_ROWS / 2) - 1); /* End column. */

	warpPrint("Frame columns: (%d -> %d)\n", (SCREEN_MAX_COLS / 2) - (FRAME_NUM_COLS / 2), (SCREEN_MAX_COLS / 2) + (FRAME_NUM_COLS / 2));
	warpPrint("Frame rows:    (%d -> %d)\n", (SCREEN_MAX_ROWS / 2) - (FRAME_NUM_ROWS / 2), (SCREEN_MAX_ROWS / 2) + (FRAME_NUM_ROWS / 2));

	SEGGER_RTT_WriteString(0, "\r\n\tDone with initialisation sequence.\n");
}