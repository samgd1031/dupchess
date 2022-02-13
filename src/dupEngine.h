#pragma once

#include <string>

#include "utils.h"
#include "board.h"
#include "move.h"

// class for the DupChess Engine
// main class to handle UCI commands as well as manage the thinking about chess part
class DupEngine {
public:
	DupEngine(); // constructor
	
	static std::string DupEngine::START_FEN;

	// informational functions
	std::string getEngineName();
	std::string getEngineVersion();
	std::string getAuthorName();
	std::string getEmailAddress();
	std::string getCountryName();

	// game state functions
	bool whiteToMove();

	// move generation
	std::vector<Move> getLegalMoves();

	//board representation
	Board gameboard;

	// talk smack to the haters
	std::string trash_talk();

private:
	std::vector<Move> mlist;

	// move generation
	inline void findPawnMoves(std::vector<Move>& mlist);

	
};