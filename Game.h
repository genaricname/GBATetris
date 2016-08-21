/**
 * Thatcher Freeman
 * Header file for Game.c
 */

//Defines location of the upper left hand corner of the positions array in terms of pixels.
//Has an additional collumn on the left as a buffer to detect collisions with the left side.
//The part of the array you can see is at (GAME_BOARD_ORIGIN_COL + BLOCK_WIDTH).
#define GAME_BOARD_ORIGIN_ROW (0)
#define GAME_BOARD_ORIGIN_COL (8)

//Defines location of next field.
#define NEXT_BLOCK_FIELD_ORIGIN_ROW (128)
#define NEXT_BLOCK_FIELD_ORIGIN_COL (168)

//Blocks are square, so this is used interchangeably with the height of the block
//in many cases.
#define BLOCK_WIDTH (8)

//gets the pixel location (0 to 160 or 240) of an index in the Positions array.
#define ROW_FROM_POSITION_INDEX(i) ((i) / GAME_WIDTH * BLOCK_WIDTH + GAME_BOARD_ORIGIN_ROW)
#define COL_FROM_POSITION_INDEX(i) ((i) % GAME_WIDTH * BLOCK_WIDTH + GAME_BOARD_ORIGIN_COL)
#define UPDATE_PERIOD(level) (40 - level)
#define FRAMES_UNTIL_REPEAT_PRESS (5)

int playing(int seed);
void playingAnimation();
void gameOver(int score);
int start();
void gameOverAnimation();
void startAnimation();
void drawPositions();
void drawPiece(Piece piece);
void erasePiece(Piece piece);
void updateNextPieceField(Piece nextPiece);
void testImage();
void copyPiece(Piece* dest, Piece* src);