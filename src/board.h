#pragma once

#include <vector>
#include <string>

#include "utils.h"


class Board {
	public:
		// holds the current state of the game
		BOARD_STATE current_state;
		
		// sets board state from FEN string
		void setBoardFromFEN(std::string FENstring);

	private:
		// holds the state of the game in the last halfmove
		BOARD_STATE last_state;

		// clears all pieces off board (all bitboards to zero), used when setting board state from FEN
		void clearBoard();
};