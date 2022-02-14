#pragma once

#include <vector>
#include <string>

#include "utils.h"


class Board {
	public:
		// holds the current state of the game
		util::BOARD_STATE current_state;
		
		// sets board state from FEN string
		void setBoardFromFEN(std::string FENstring);

	private:
		// clears all pieces off board (all bitboards to zero), used when setting board state from FEN
		void clearBoard();
};