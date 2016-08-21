/**
 * Thatcher Freeman
 * 
 * Array of tetriminoes.
 * Piece Structure definition is in "GameBackend.h" 
 * Information on what the fields mean is in the getPositionsOfPieces() function in "GameBackend.c"
 */
#include "GameBackend.h"
#include "myLib.h"
#include "Blocks.h"


Piece Pieces[7] = {
//O
{
	YellowBlock,
	GAME_PIECE_DEFAULT_ROW,
	GAME_PIECE_DEFAULT_COL,
	0,
	{{5, 1, 4, 0},
	{5, 1, 4, 0},
	{5, 1, 4, 0},
	{5, 1, 4, 0}}
},

//I
{
	CyanBlock,
	GAME_PIECE_DEFAULT_ROW,
	GAME_PIECE_DEFAULT_COL,
	0,
	{{5, 1, 9, 13},
	{5, 4, 6, 7},
	{5, 1, 9, 13},
	{5, 4, 6, 7}}
},

//S
{
	GreenBlock,
	GAME_PIECE_DEFAULT_ROW,
	GAME_PIECE_DEFAULT_COL,
	0,
	{{5, 1, 2, 4},
	{5, 1, 6, 10},
	{5, 6, 8, 9},
	{5, 0, 4, 9}}
},

//Z
{
	RedBlock,
	GAME_PIECE_DEFAULT_ROW,
	GAME_PIECE_DEFAULT_COL,
	0,
	{{5, 0, 1, 6},
	{5, 2, 6, 9},
	{5, 4, 9, 10},
	{5, 1, 4, 8}}
},

//T
{
	PurpleBlock,
	GAME_PIECE_DEFAULT_ROW,
	GAME_PIECE_DEFAULT_COL,
	0,
	{{5, 4, 6, 9},
	{5, 1, 4, 9},
	{5, 1, 4, 6},
	{5, 1, 6, 9}}
},

//J
{
	BlueBlock,
	GAME_PIECE_DEFAULT_ROW,
	GAME_PIECE_DEFAULT_COL,
	0,
	{{5, 1, 8, 9},
	{5, 0, 4, 6},
	{5, 1, 2, 9},
	{5, 4, 6, 10}}
},

//L
{
	OrangeBlock,
	GAME_PIECE_DEFAULT_ROW,
	GAME_PIECE_DEFAULT_COL,
	0,
	{{5, 1, 9, 10},
	{5, 4, 6, 8},
	{5, 0, 1, 9},
	{5, 2, 4, 6}}
}};
