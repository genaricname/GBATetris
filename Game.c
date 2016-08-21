/**
 * Thatcher Freeman
 * File with the code that runs the game and drawing things on the screen.
 */
#include "GameBackend.h"
#include "myLib.h"
#include "GameplayBackground.h"
#include "GameOver.h"
#include "GameOverBlock.h"
#include "Game.h"
#include "Start.h"
#include "StartArrow.h"
#include "StartBlock.h"
#include "StartArrowBackground.h"
#include "PlayingBlock.h"
#include "Pieces.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Blocks.h"
#include "Paused.h"

/**
 * Executes during for start state of the game.
 * @return the number of frames spent on the start screen, for use as a seed
 *            in the random number generator for the pieces.
 */
int start() {
	//Do the cool opening animation.
	startAnimation();
	
	//Draw the start screen
	waitforVBlank();
	drawImage3(0, 0, START_WIDTH, START_HEIGHT, Start);

	//Start with arrow on (personal preference)
	drawImage3(109, 80, STARTARROW_WIDTH, STARTARROW_HEIGHT, StartArrow);
	int arrowOn = 1;
	
	//Start frame counter.
	int frameCounter = 0;

	//Wait until the buttons are released
	int keysReleased = 0;
	
	while (1) {
		frameCounter++;
		waitforVBlank();

		//Blink Arrow next to "start"
		if (!(frameCounter % 200)){
			if (arrowOn) {
				//Erase arrow if already drawn.
				drawImage3(104, 76, STARTARROWBACKGROUND_WIDTH, STARTARROWBACKGROUND_HEIGHT, StartArrowBackground);
				arrowOn = 0;
			} else {
				//Draw arrow if not present.
				drawImage3(109, 80, STARTARROW_WIDTH, STARTARROW_HEIGHT, StartArrow);
				arrowOn = 1;
			}
		}

		//Wait until all the keys are released.
		if (keysReleased && (KEY_DOWN_NOW(BUTTON_A) || KEY_DOWN_NOW(BUTTON_START))) {
			//If A or Start is pressed, start the game.
			break;
		} else if (!KEY_DOWN_NOW(BUTTON_A) && !KEY_DOWN_NOW(BUTTON_START)) {
			keysReleased = 1;
		}
	}

	return frameCounter;
}

/**
 * Animation before the start sequence.
 */
void startAnimation() {
	for (int r = 160 - STARTBLOCK_HEIGHT; r >= 0; r = r - STARTBLOCK_HEIGHT) {
		for (int c = 0; c <= 240 - STARTBLOCK_WIDTH; c = c + STARTBLOCK_WIDTH) {
			waitforVBlank();
			drawImage3(r, c, STARTBLOCK_WIDTH, STARTBLOCK_HEIGHT, StartBlock);
		}
	}
}

/**
 * Executes during Playing state of the game.
 * @param  seed A seed for the random generator that selects which piece is next.
 * @return      Your score.
 */
int playing(int seed) {
	
	//Important game-relevent values
	int paused = 0;
	int score = 0;
	int level = 0;

	//Setup gameplay
	playingAnimation();
	srand(seed);

	//Clear the table of block positions
	gameRestart();
	
	//Generate useful pieces.
	Piece* next = (Piece*) malloc(sizeof(Piece)); //Piece that is drawn in the "next" field
	Piece* curr = (Piece*) malloc(sizeof(Piece)); //Piece that is drawn on the screen
	Piece* temp = (Piece*) malloc(sizeof(Piece)); //Piece that probes the next spot to see if it's occupied
	Piece* old = (Piece*) malloc(sizeof(Piece));  //Piece that gets erased

	if (next == NULL || curr == NULL || temp == NULL || old == NULL) {
		return -1;
	}

	copyPiece(next, &Pieces[rand() % PIECES_LENGTH]);
	copyPiece(curr, &Pieces[rand() % PIECES_LENGTH]);
	
	//Drawing in initial parts of the screen
	waitforVBlank();
	drawImage3(0, 0, GAMEPLAYBACKGROUND_WIDTH, GAMEPLAYBACKGROUND_HEIGHT, GameplayBackground);
	drawNums3(22, 155, score);
	drawNums3(70, 155, level);
	updateNextPieceField(*next);

	//Used to store the previous frame right before the pause screen is displayed.
	unsigned short* oldFrame = (unsigned short*) malloc(240 * 160 * sizeof(short));
	
	//Used to keep track of which buttons are currently being held down.
	int buttonsReleased[10] = {0};
	/**
	 * [0] = Button A
	 * [1] = Button B
	 * [2] = Select
	 * [3] = Start
	 * [4] = Right
	 * [5] = Left
	 * [6] = Up
	 * [7] = Down
	 * [8] = R
	 * [9] = L
	 */

	//Used to keep track of time.
	int frameCounter = 0;

	//Play the game:
	while (1) {
		if (KEY_DOWN_NOW(BUTTON_SELECT)) {
			//Return to start screen. Escapes the Playing state.
			score = -1;
			break;
		}

		//Do nothing if the game is paused.
		if (paused) {
			if (!KEY_DOWN_NOW(BUTTON_START)) {

				//Check if start has been released
				buttonsReleased[3] = 0;

			} else if (!buttonsReleased[3] && KEY_DOWN_NOW(BUTTON_START)) {

				//Unpause the game.
				paused = 0;
				buttonsReleased[3] = 1;

				//Draw old frame on top of the pause screen that's currently displayed.
				waitforVBlank();
				drawImage3(0, 0, 240, 160, oldFrame);

			}
		} else {
			//Play the game (it's not paused)
			
			//Store value of current piece
			copyPiece(old, curr);

			//Duplicate current piece into temp variable.
			copyPiece(temp, curr);
			
			//Increment framecounter.
			frameCounter = 1 + (frameCounter % UPDATE_PERIOD(level));

			if (frameCounter == 1) {
				//Move piece downwards
				temp->r = temp->r + 1;
				if (isOccupied(*temp)) {
					//Spot below curr is occupied. Freeze curr in current position and stop it.

					//Update score and level after placing piece
					score += placePiece(*curr);
					level = score / 150;

					//Update current and next pieces
					copyPiece(curr, next);
					copyPiece(next, &Pieces[rand() % PIECES_LENGTH]);

					//Update score and next fields, update board to reflect
					//removed lines etc, draw new current piece.
					waitforVBlank();
					updateNextPieceField(*next);
					drawPositions();
					drawPiece(*curr);
					//draw the numbers last in case the other stuff takes too long, 
					//nobody will notice tearing in the score.
					drawNums3(22, 155, score);
					drawNums3(70, 155, level);
					

					if (isOccupied(*curr)) {
						//Game board is pretty full, Game over.
						break;
					}

					//Piece has been set, restart the loop.
					continue;

				}
			}

			/* Check various buttons and move temp to that location */
			if (!buttonsReleased[0] && KEY_DOWN_NOW(BUTTON_A)) {
				//Rotate piece clockwise.
				temp->orientation = (temp->orientation + 1) % 4;
				buttonsReleased[0] = 1;
			} else if (!KEY_DOWN_NOW(BUTTON_A)) {
				buttonsReleased[0] = 0;
			}

			if (!buttonsReleased[1] && KEY_DOWN_NOW(BUTTON_B)) {
				//Rotate piece counterclockwise
				temp->orientation = (temp->orientation + 3) % 4;
				buttonsReleased[1] = 1;
			} else if (!KEY_DOWN_NOW(BUTTON_B)) {
				buttonsReleased[1] = 0;
			}

			if (!buttonsReleased[5] && KEY_DOWN_NOW(BUTTON_LEFT)) {
				//Move Piece left
				temp->c = temp->c - 1;
				buttonsReleased[5] = 1;
			} else if (!KEY_DOWN_NOW(BUTTON_LEFT)) {
				buttonsReleased[5] = 0;
			}

			if (!buttonsReleased[4] && KEY_DOWN_NOW(BUTTON_RIGHT)) {
				//Move piece right
				temp->c = temp->c + 1;
				buttonsReleased[4] = 1;
			} else if (!KEY_DOWN_NOW(BUTTON_RIGHT)) {
				buttonsReleased[4] = 0;
			}

			if (!buttonsReleased[7] && KEY_DOWN_NOW(BUTTON_DOWN)) {
				//Move piece down
				temp->r = temp->r + 1;
				buttonsReleased[7] = 1;
			} else if (!KEY_DOWN_NOW(BUTTON_B)) {
				buttonsReleased[7] = 0;
			}
			
			//Pause game if Start is pressed.
			if (!buttonsReleased[3] && KEY_DOWN_NOW(BUTTON_START)) {
				buttonsReleased[3] = 1;

				//Pause game
				paused = 1;

				//Store current frame so that it can be redrawn later.
				storeFrame3(oldFrame);

				//Draw pause screen.
				waitforVBlank();
				drawImage3(0, 0, PAUSED_WIDTH, PAUSED_HEIGHT, Paused);

				//Restart loop so it defaults to the paused section.
				continue;
			} else if (!KEY_DOWN_NOW(BUTTON_START)) {
				buttonsReleased[3] = 0;
			}



			if (isOccupied(*temp)) {
				
				//If the probe position is occupied, then do NOT move
				//the current piece to its position.
				waitforVBlank();
			
			} else {
				//Move current piece to probe position.
				copyPiece(curr, temp);

				//Erase old position, draw new one.
				waitforVBlank();
				erasePiece(*old);
				drawPiece(*curr);
			}
		}
	}
	free(oldFrame);
	free(curr);
	free(old);
	free(temp);
	free(next);
	//Return the score for game over screen.
	return score;
}

/**
 * Animation that plays before gameplay begins.
 */
void playingAnimation() {
	for (int r = 160 - PLAYINGBLOCK_HEIGHT; r >= 0; r = r - PLAYINGBLOCK_HEIGHT) {
		for (int c = 0; c <= 240 - PLAYINGBLOCK_WIDTH; c = c + PLAYINGBLOCK_WIDTH) {
			waitforVBlank();
			drawImage3(r, c, PLAYINGBLOCK_WIDTH, PLAYINGBLOCK_HEIGHT, PlayingBlock);
		}
	}
}

/**
 * Executes during game over state
 * @param score the score to be drawn onto the screen
 */
void gameOver(int score) {
	gameOverAnimation();

	//Display the gameOver screen with the score at time of losing.
	waitforVBlank();
	drawImage3(0,0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, GameOver);
	drawNums3(83, 93, score);

	while(~BUTTONS & 0xFF); 		//do nothing until all buttons are released
	while(!(~BUTTONS & 0xFF)); 		//do nothing until a new button is pressed.
}

/**
 * Animation that plays before Game Over screen.
 */
void gameOverAnimation() {
	for (int r = 160 - GAMEOVERBLOCK_HEIGHT; r >= 0; r = r - GAMEOVERBLOCK_HEIGHT) {
		for (int c = 0; c <= 240 - GAMEOVERBLOCK_WIDTH; c = c + GAMEOVERBLOCK_WIDTH) {
			waitforVBlank();
			drawImage3(r, c, GAMEOVERBLOCK_WIDTH, GAMEOVERBLOCK_HEIGHT, GameOverBlock);
		}
	}
}

/**
 * Copy the data from one piece to another. Try not to use in time
 * sensitive locations, might be inefficient.
 * @param dest The piece whose data is being chagned.
 * @param src  The source piece whose data is copied
 */
void copyPiece(Piece* dest, Piece* src) {
	/*
	DMA[DMA_CHANNEL_3].src = &src;
	DMA[DMA_CHANNEL_3].dst = &dest;
	DMA[DMA_CHANNEL_3].cnt = sizeof(Pieces[0]) |
								DMA_SOURCE_INCREMENT |
								DMA_DESTINATION_INCREMENT |
								DMA_ON;
	//Above code causes malloc to fail during testing. Absolutely no clue why.
	//Maybe a volatile thing?
	*/

	//Just copy the piece over with memcpy. Hope it's smart enough to use DMA for this.
	memcpy(dest, src, sizeof(Pieces[0]));
}

/**
 * Redraw all the blocks in the game board (does not include the current piece because
 * pieces are only stored in Position when they hit the bottom of the board.)
 */
void drawPositions() {
	for (int i = 0, r = GAME_BOARD_ORIGIN_ROW; i < GAME_HEIGHT - 1; i++, r += BLOCK_WIDTH) {
		for (int j = 1, c = GAME_BOARD_ORIGIN_COL + BLOCK_WIDTH; j < GAME_WIDTH - 1; j++, c += BLOCK_WIDTH) {
			drawImage3(r, c, BLOCK_WIDTH, BLOCK_WIDTH, positions[GAME_ABS_POS(i,j)]);
		}
	}
}

/**
 * Draws a single piece on the screen.
 * @param piece 	the piece to be drawn
 */
void drawPiece(Piece piece) {
	int piecePositions[4];
	getPositionsOfPiece(piecePositions, piece);
	for (int i = 0; i < 4; i++) {
		//Change to drawImage3 if I set up pieces to be made of icons.
		drawImage3(ROW_FROM_POSITION_INDEX(piecePositions[i]), COL_FROM_POSITION_INDEX(piecePositions[i]),
					BLOCK_WIDTH, BLOCK_WIDTH, piece.style);
	}
}

/**
 * Draws over a piece on the screen with the background block defined in Blocks.h,
 * effectively erasing it
 * @param piece 	The piece to be erased.
 */
void erasePiece(Piece piece) {
	int piecePositions[4];
	getPositionsOfPiece(piecePositions, piece);
	for (int i = 0; i < 4; i++) {
		drawImage3(ROW_FROM_POSITION_INDEX(piecePositions[i]), COL_FROM_POSITION_INDEX(piecePositions[i]),
					BLOCK_WIDTH, BLOCK_WIDTH, BlackBlock);
	}
}

/**
 * Draws a piece in the next field during gameplay.
 * @param nextPiece 	The piece to be drawn
 */
void updateNextPieceField(Piece nextPiece) {

	//Erase whatever is currently in the next field.
	//0x0677F is the color I selected to be the background color
	//of the numbers and next fields in the game.
	drawRect3(108, 152, 39, 39, 0x0677F);
	
	//Essentially a rewritten version of getPositionsOfPiece() but adjusted to
	//draw the piece in the field on the side of the screen.
	
	//Read through getPositionsOfPiece() for an explanation of how the positioning
	//system works in this game.
	int* nextPiecePositions = nextPiece.relativePositions[nextPiece.orientation];
	int nextPieceStartRow = GAME_REL_POS_ROW(nextPiecePositions[0]);
	int nextPieceStartCol = GAME_REL_POS_COL(nextPiecePositions[0]);
	
	for (int i = 0; i < 4; i++) {
		int p = nextPiecePositions[i];
		int aRow = (GAME_REL_POS_ROW(p) - nextPieceStartRow) * BLOCK_WIDTH + 120;
		int aCol = (GAME_REL_POS_COL(p) - nextPieceStartCol) * BLOCK_WIDTH + 168;
		drawImage3(aRow, aCol, BLOCK_WIDTH, BLOCK_WIDTH, nextPiece.style);
	}

}

/**
 * Testing method.
 * inaccessable in game.
 */
void testImage() {
	//Method for testing other methods. Not used in game.
	startAnimation();
	gameOverAnimation();
	playingAnimation();

	Piece* curr = (Piece*) malloc(sizeof(Piece));
	Piece* old = (Piece*) malloc(sizeof(Piece));

	copyPiece(curr, &Pieces[6]);
	copyPiece(old, &Pieces[5]);

	curr->r = 5;
	old->r = 5;
	curr->c = 6;
	old->c = 6;
	curr->orientation = 0;
	old->orientation = 0;
	curr->style = YellowBlock;
	old->style = YellowBlock;

	waitforVBlank();
	drawImage3(0,0,GAMEPLAYBACKGROUND_WIDTH, GAMEPLAYBACKGROUND_WIDTH, GameplayBackground);

	drawPiece(*curr);
	drawPiece(*old);

	drawNums3(22, 155, NEXT_BLOCK_FIELD_ORIGIN_COL * 2 + 84);
	drawNums3(70, 155, 5 + ROW_FROM_POSITION_INDEX(96));

	while (1);
	

	free(old);
	free(curr);
}	