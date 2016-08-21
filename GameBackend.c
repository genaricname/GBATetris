/**
 * Thatcher Freeman
 * Code that controls the positions of the blocks on the screen.
 */
#include "GameBackend.h"
#include <string.h>
#include "myLib.h"
#include "Pieces.h"
#include <stdlib.h>
#include <stdio.h>
#include "Blocks.h"

//Contains the visual elements and their positions in the board. Origin is in the upper
//left hand corner of the board. There is a buffer collumn on the left and right sides
//of the board and a buffer row on the bottom used in collision detection. As a result,
//I've actually defined the origin of positions to be one space to the left of the
//upper left pixel of the visible game board.
//
//I've also defined rows to to increase going down from the origin and collumns to
//increase going to the right of the origin
Style positions[GAME_WIDTH * GAME_HEIGHT] = {BlackBlock};

/**
 * Clears whatever elements are in positions to their default values, BlackBlock.
 */
void gameRestart() {
	/*
	DMA[DMA_CHANNEL_3].src = BlackBlock;
	DMA[DMA_CHANNEL_3].dst = positions;
	DMA[DMA_CHANNEL_3].cnt = (GAME_WIDTH * GAME_HEIGHT) |
								DMA_SOURCE_FIXED |
								DMA_DESTINATION_INCREMENT |
								DMA_ON;
	
	*/

	for (int i = 0; i < GAME_WIDTH * GAME_HEIGHT; i++) {
		positions[i] = BlackBlock;
	}

	//memset(positions, BlackBlock, sizeof(positions));
}

/**
 * Determines if any of the blocks in the probe piece will collide
 * with an existing block in the array positions.
 * @param  probe The test piece
 * @return       1 if the probe will collide with the wall or an existing block
 *               0 if the probe piece can move there legally.
 */
int isOccupied(Piece probe) {
	int occupiedPositions[4] = {0};
	getPositionsOfPiece(occupiedPositions, probe);
	
	int isOccupied = 0;

	for (int i = 0; i < 4; i++) {
		//Check if the piece is colliding with the left, bottom, or right walls.
		if ((occupiedPositions[i] % GAME_WIDTH == 0
					|| occupiedPositions[i] % GAME_WIDTH == (GAME_WIDTH - 1))
					|| occupiedPositions[i] / GAME_WIDTH == (GAME_HEIGHT - 1)
					|| occupiedPositions[i] < 0) {
			//intersects with borders.
			isOccupied = 1;
		} else if (positions[occupiedPositions[i]] - BlackBlock != 0) {
			//intersects with occupied space.
			isOccupied = 1;
		}
	}
	return isOccupied;
}

/**
 * Gets the indices in positions[] of the blocks in piece.
 * @param absolutePositions The array to be populated with these positions.
 * @param piece             The piece whose positions you're looking for.
 *
 * I've defined a matrix below from which I've derived all the "relativePositions"
 * stored in each Piece structure.
 * 
 *  --- --- --- ---
 * | 0 | 1 | 2 | 3 |
 *  --- --- --- ---
 * | 4 | 5 | 6 | 7 |
 *  --- --- --- ---
 * | 8 | 9 | 10| 11|
 *  --- --- --- ---
 * | 12| 13| 14| 15|
 *  --- --- --- ---
 *
 * Every orientation of each piece can be contained in this 4x4 matrix. In each piece,
 * I have a pair of values, (r, c) which represent the coordinates of position 5 in this matrix
 * on the full game board.
 *
 * The matrix relativePositions has four arrays, one for each of the four possible orientations of
 * the piece. In each of those arrays, I have four numbers that represent the four spots in the
 * matrix I have just written above that the piece covers. If you have a T-block, for example,
 * you could say that it covers {1, 4, 5, 6} in the matrix I've drawn above, and in its next orientation
 * (turning it clockwise 90 degrees), it covers {1, 5, 6, 9}, if we're using spot 5 as an origin/pivot
 * position.
 *
 * In the following function, I translate one of these four element arrays of relativePositions into
 * the their corresponding spots on the board, where relativePositions[orientation][0] is at the board
 * position (r,c).
 *
 * This is possible because if you rewrite my matrix in binary, the first two bits of each spot
 * represent the row number and the last two bits represent the column number. From there, I find the
 * row and column number of the origin block (the one at relativePositions[orientation][0]) and I can
 * then determine if the other blocks in relativePositions[orientation] are one or two rows or columns
 * above or below the origin block. Then, I can determine the positions of all the blocks in the in the
 * board.
 *
 * All of this is so that you can look at a piece at any position and see which spots in the array, 
 * positions, it would occupy if placed. From there, you can see if those spots are occupied or whatever
 * for hit detection and whatnot.
 */
void getPositionsOfPiece(int absolutePositions[], Piece piece) {

	//Get the relative positions (values from 0 to 15) of the piece in its current orientation.
	int* relativePositions = (int*)piece.relativePositions[piece.orientation];

	//Fetch the relative position of the origin block, which I've defined as the block
	//whose relative position is marked in piece.relativePositions[orientation][0].
	int rRow = GAME_REL_POS_ROW(relativePositions[0]);
	int rCol = GAME_REL_POS_COL(relativePositions[0]);

	//I define the absolute position of the origin block to be at (r, c)
	absolutePositions[0] = GAME_ABS_POS(piece.r, piece.c);

	for (int i = 1; i < 4; i++) {
		//Run through each element in the relative positions at this orientation.
		int pos = relativePositions[i];

		//(GAME_REL_POS_ROW(pos) - rRow) represents the number of rows
		//above or below the origin block that this block is at. We add
		//piece.r to this to find out which row in the board this corresponds to.
		int aRow = GAME_REL_POS_ROW(pos) - rRow + piece.r;
		//Same deal here as finding the row.
		int aCol = GAME_REL_POS_COL(pos) - rCol + piece.c;

		//absolutePositions[i] represents the index in positions that corresponds
		//to the position of this block.
		absolutePositions[i] = GAME_ABS_POS(aRow, aCol);
	}
}

/**
 * Place the piece in its current location, then check for completed lines
 * and return the points earned in this action.
 * @param  piece the piece to be placed 
 * @return       The number of points earned.
 */
int placePiece(Piece piece) {
	//give you five points for placing a piece.
	int pointsEarned = 5;

	//Stores the indices in positions where
	//piece will be placed.
	int absolutePositions[4];
	getPositionsOfPiece(absolutePositions, piece);

	//You can delete up to four rows at a time.
	int rowsToRemove[4];

	for (int i = 0; i < 4; i++) {
		//Get the row number of all the rows where
		//blocks in this piece have been placed.
		rowsToRemove[i] = absolutePositions[i] / GAME_WIDTH;
		
		//Add the blocks to positions
		positions[absolutePositions[i]] = piece.style;
	}

	//Sort the rows in ascending order. Removing rows from the top of
	//the game board (low row numbers) to the bottom (high row numbers)
	//helps avoid bugs where the row number of a line you want to remove
	//changes because you removed a line in the board below it.
	insertionSort(rowsToRemove);

	//Give you twenty points for each line removed, remove lines from
	//the top, downwards like described above.
	for (int i = 0; i < 4; i++) {
		pointsEarned += 20 * checkForCompletedLine(rowsToRemove[i]);
	}
	
	//return the pints earned in placing this piece.
	return pointsEarned;
}

/**
 * Simple insertion sort used to sort the array rowsToRemove in 
 * placePiece(). Implementation stolen from Wikipedia.
 * @param ints Array of elements to sort. (Must be 4 long!!!)
 */
void insertionSort(int* ints) {
	//i < 4 is simply hardcoded because I'm not going to use
	//this with any arrays longer than 4.
	for (int i = 1, j; i < 4; i++) {
		int x = ints[i];
		for (j = i - 1; ints[j] > x; j--) {
			ints[j + 1] = ints[j];
		}
		ints[j + 1] = x;
	}
}

/**
 * Checks the row to see if the whole row on the board is filled.
 * @param  row The row number of the row you want to check
 * @return     1 if the row happened to be a complete line and has been removed.
 *             0 if the row was not a complete line.
 */
int checkForCompletedLine(int row) {
	int isLine = 1;
	for (int c = 1; c < GAME_WIDTH - 1; c++) {
		if (positions[GAME_ABS_POS(row, c)] - BlackBlock == 0) {
			//If there are empty spots in the row, it is not a line.
			//return zero.
			isLine = 0;
			return isLine;
		}
	}

	//The row must be a complete line, remove the row.
	removeCompletedLines(row, row);
	
	//Return that it was a line.
	return isLine;
}

/**
 * Removes lines. Initially designed for removing multiple lines in a row,
 * but I felt like that would be too specific so I never actually use it that
 * way in the code. As a result, I only use it to delete one row at a time.
 * The compiler will probably optimize most of this stuff out, so I'm not 
 * too worried about efficiency.
 * @param low  inclusive row number of the top most row of the game board 
 *             	you want to eliminate.
 * @param high inclusive row number of the bottom most row of the game board
 *             you want to eliminate.
 */
void removeCompletedLines(int low, int high) {
	//The number of rows to remove, added one because low and high are inclusive
	//indices
	int rowsRemoved = high - low + 1;
	
	for (int p = GAME_ABS_POS(high - rowsRemoved, GAME_WIDTH - 1); p >=0; p--) {
		//Shift all the elements in positions[] above this row downwards, overwriting
		//this row.
		positions[p + GAME_WIDTH * rowsRemoved] = positions[p];
	}
	for (int p = GAME_WIDTH * rowsRemoved; p >=0; p--) {
		//Just to be sure, fill in the top rows that were copied downwards with
		//the background block to make sure they're erased. Otherwise you might have
		//issues if a row near the very top is deleted and there are blocks above it.
		positions[p] = BlackBlock;
	}
}