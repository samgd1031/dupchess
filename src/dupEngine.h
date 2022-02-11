#pragma once

#include "utils.h"
#include "board.h"

// class for the DupChess Engine
// main class to handle UCI commands as well as manage the thinking about chess part
class DupEngine {
public:
	DupEngine(); // constructor
	
	static const std::string START_FEN;

	// informational functions
	std::string getEngineName();
	std::string getEngineVersion();
	std::string getAuthorName();
	std::string getEmailAddress();
	std::string getCountryName();

	// game state functions
	bool whiteToMove();

private:
	//board representation
	Board gameboard;

	// move generation
	std::vector<std::string> getLegalMoves();

	
};