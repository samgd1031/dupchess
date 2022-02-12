#include "move.h"


// Constructor
Move::Move(uint32_t data) {
	bits = data;
}

// return a long SAN notation string for the move
std::string Move::getLongSAN() {
	std::string san = "";
	
	// if not a pawn put the piece abbreviation
	if ((bits & pieceMask) >> pieceShift > 0) {
		san += pieceAbbr[(bits & pieceMask) >> pieceShift];
	}
	// append the origin square
	san += squareStrings[bits & fromMask];
	
	// add an x if a capture
	if ((bits & capMask) >> capShift == 1) {
		san += "x";
	}
	// add the destination square
	san += squareStrings[(bits & toMask) >> toShift];

	// add the promotion
	if ((bits & promoMask) >> promoShift == 1) {
		// get piece promoting to
		san += "=" + pieceAbbr[(bits & utilMask) >> utilShift];
	}

	return san;
}