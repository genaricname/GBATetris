/**
 * Thatcher Freeman
 *
 * Library functions useful in GBA games.
 * Many of these were done in class.
 */
#include "myLib.h"
#include "Numbers.h"

//Address of GBA videoBuffer.
unsigned short* videoBuffer = (unsigned short*)0x6000000;

/**
 * Draws an image to a position on the gba screen
 * @param r      Row number of destination position on screen
 * @param c      Collumn number of destination position on screen
 * @param width  Width of image
 * @param height Height of image
 * @param image  the image to be drawn
 */
void drawImage3 (int r, int c, int width, int height, const u16* image) {
	for (int row = r; row < r + height && row < 160; row++) {
		drawRow(row, c, row - r, width, image);
	}
}

/**
 * Efficiently draws out one row of pixels from the image to the screen, uses
 * a DMA for efficiency.
 * @param r        the row on the screen where the row should be drawn
 * @param c        starting collumn on the screen where the line of pixels should be drawn
 * @param rowCount the row number in the picture to be drawn
 * @param width    the width of the image
 * @param image    the image to be drawn
 */
void drawRow(int r, int c, int rowCount, int width, const u16* image) {

	DMA[DMA_CHANNEL_3].src = image + rowCount * width;
	DMA[DMA_CHANNEL_3].dst = videoBuffer + r * 240 + c;
	DMA[DMA_CHANNEL_3].cnt = width |
								DMA_SOURCE_INCREMENT |
								DMA_DESTINATION_INCREMENT |
								DMA_ON;
}

/**
 * Waits until the next time the scan line counter is off screen.
 * Prevents tearing.
 */
void waitforVBlank() {
	while (SCANLINECOUNTER > 160);
	while (SCANLINECOUNTER < 160);
}

/**
 * Draws the number to the screen in the given position,
 * writes it as a 0 padded six digit number.
 *
 * Numbrs is pre-encoded to have a tan background because
 * that's the color I made all the number field backgrounds.
 * @param r      The row number on the screen where the digits are to be drawn
 * @param c      the collumn number on the screen where the digits are to be drawn
 * @param number the number to be drawn (between 000,000 and 999,999).
 */
void drawNums3(int r, int c, int number) {

	//Start on the right side.
	int col = c + (NUMBER_LENGTH - 1) * (NUMBERS_WIDTH + 2);

	for (int i = 0; i < NUMBER_LENGTH; i++) {
		//get the last digit of the number
		int digit = number % 10;
		number = number / 10;

		//draw the last digit of the number.
		drawDigit(r, col, digit);

		//Decrement col so that you can draw the
		//next to last digit of the number.
		col = col - (NUMBERS_WIDTH + 2);
	}

}

/**
 * Draw a digit to the screen, basically an implementation of drawImage with a specific image.
 * @param r     Row where the number is to be drawn
 * @param c     col where the number is to be drawn
 * @param digit the number to be drawn (between 0 and 9).
 */
void drawDigit(int r, int c, int digit) {
	drawImage3(r, c, NUMBERS_WIDTH, NUMBERS_HEIGHT, (Numbers + digit * NUMBERS_WIDTH * NUMBERS_HEIGHT));
}

/**
 * Draw a rectangle on the gba screen, uses a DMA for efficiency.
 * @param r      row coordinate where the rectangle is to be drawn
 * @param c      col coordinate where the rectangle is to be drawn
 * @param width  the width of the rectangle
 * @param height the height of the rectangle
 * @param color  the color of the rectangle
 */
void drawRect3(int r, int c, int width, int height, u16 color) {
	for (int row = r; row < r + height && row < 160; row++) {
		DMA[DMA_CHANNEL_3].src = &color;
		DMA[DMA_CHANNEL_3].dst = videoBuffer + row * 240 + c;
		DMA[DMA_CHANNEL_3].cnt = width |
								DMA_SOURCE_FIXED |
								DMA_DESTINATION_INCREMENT |
								DMA_ON;
	}
}

/**
 * Copies the data from the current frame on the videoBuffer
 * over to the array in the parameter.
 * @param dest 		pointer where you want to save the current frame to.
 */
void storeFrame3(unsigned short* dest) {
	DMA[DMA_CHANNEL_3].src = videoBuffer;
	DMA[DMA_CHANNEL_3].dst = dest;
	DMA[DMA_CHANNEL_3].cnt = (240 * 160) |
								DMA_SOURCE_INCREMENT |
								DMA_DESTINATION_INCREMENT |
								DMA_ON;
}