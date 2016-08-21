/**
 * Thatcher Freeman
 * 
 * The tetris game. Basically a state machine.
 */
#include "myLib.h"
#include "GameplayBackground.h"
#include "GameOver.h"
#include "GameBackend.h"
#include "Game.h"
#include "Pieces.h"

//The allowed states.
enum gameState {
	START,
	GAME_OVER,
	PLAYING,
	TEST_IMAGE,
};

/**
 * Method that runs the game
 * @return Exit code, not at all relevant in gameboy games.
 */
int main () {
	//Start off the game
	enum gameState state;
	state = START;

	//Mode 3.
	REG_DISPCNT = MODE_3 | BG2_EN;
	
	//Starting off variables that are passed between functions.
	int score = 0;
	int seed = 0;


	while (1) {
		switch (state) {

			case START:
				//Start screen state.
				seed = start();
				state = PLAYING;
				break;
			
			case PLAYING:
				//In-game state.
				score = playing(seed);
				if (score == -1) {
					//Indicates that you pressed the reset button
					//(select) during gameplay.
					state = START;
				} else {
					state = GAME_OVER;
				}
				break;

			case GAME_OVER:
				//GameOver state.

				//Every button returns the game back to the start
				//screen, so coding the reset button in here
				//would be redundant.
				gameOver(score);
				state = START;
				break;

			case TEST_IMAGE:
				//Unused state during gameplay.
				testImage();
				break;
		}
	}
	return 1;
}


