/**
 * Thatcher Freeman
 * Metadata for Pieces[].
 *
 * Piece Structure definition is in "GameBackend.h" 
 * Information on what the fields mean is in the getPositionsOfPieces() function in "GameBackend.c"
 */


//Number of pieces is currently hardcoded because the line below gave a "dividing by zero" error at some point.
//Haven't bothered fixing this yet.
#define PIECES_LENGTH 7
// #define PIECES_LENGTH (sizeof(Pieces) / sizeof(Pieces[0]))

//Array of possible Tetriminoes defined in Tetris.
extern Piece Pieces[];