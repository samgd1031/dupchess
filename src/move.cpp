#include "move.h"


// Constructor
Move::Move(uint32_t data) {
	bits = data;
}

// return a PGN string for the move
std::string Move::getPGN() {
	std::string pgn = "";
	
	// if not a pawn put the piece abbreviation
	if ((bits & pieceMask) >> pieceShift > 0) {
		pgn += pieceAbbr[(bits & pieceMask) >> pieceShift];
	}
	// append the origin square
	pgn += squareStrings[bits & fromMask];
	
	// add an x if a capture
	if ((bits & capMask) >> capShift == 1) {
		pgn += "x";
	}
	// add the destination square
	pgn += squareStrings[(bits & toMask) >> toShift];

	return pgn;
}