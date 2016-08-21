/**
 * Thatcher Freeman
 * Header file for GameBackend.c
 */

//Gives you the index in positions of the game element at (r,c).
#define GAME_ABS_POS(r, c) ((r) * GAME_WIDTH + (c))

//Used to find the relative row and column numbers as described in
//getPositionsOfPiece().
#define GAME_REL_POS_ROW(p) (((p) & 0xC) >> 2)
#define GAME_REL_POS_COL(p) (((p) & 0x3) >> 0)

//Board dimensions, width has a collumn of buffer on each side,
//height has a row of buffer on the bottom.
#define GAME_WIDTH (12)
#define GAME_HEIGHT (21)

//Start of new pieces sort of in the upper middle position
//in the board.
#define GAME_PIECE_DEFAULT_ROW 1
#define GAME_PIECE_DEFAULT_COL 6

//Style is a pointer to an image of a block, like the ones
//in Block.h/.c
typedef const unsigned short* Style;

//Contains the data in a piece. All covered in my comments
//in getPositionsOfPiece in the "GameBackend.c" file.
typedef struct {
	Style style;
	int r;
	int c;
	int orientation;
	int relativePositions[4][4];
} Piece;

//Represents all the blocks on the game board.
extern Style positions[GAME_WIDTH * GAME_HEIGHT];

void getPositionsOfPiece(int absolutePositions[], Piece piece);
int isOccupied(Piece probe);
int placePiece(Piece piece);
int checkForCompletedLine(int position);
void insertionSort(int* ints);
void removeCompletedLines(int low, int high);
void gameRestart();