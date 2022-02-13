#include "move.h"


// Constructor
Move::Move(uint32_t data) {
	bits = data;
}

Move::Move(uint32_t fromInd, uint32_t toInd, bool isCap, bool isPromo, bool isEP) {
	bits = fromInd;
	bits |= toInd << toShift;
	if (isCap) bits |= capMask;
	if (isPromo) bits |= promoMask;
	if (isEP) {
		bits |= epMask;						// set bit to encode this as an en passant move
		bits |= (toInd - 8) << epShift;		// set util bits to have index of enpassant square
	}
}


// return a long SAN notation string for the move
std::string Move::getLongSAN() {
	std::string san = "";
	
	// if not a pawn put the piece abbreviation
	if ((bits & pieceMask) >> pieceShift > 0) {
		san += util::pieceAbbr[(bits & pieceMask) >> pieceShift];
	}
	// append the origin square
	san += util::squareStrings[bits & fromMask];
	
	// add an x if a capture
	if ((bits & capMask) >> capShift == 1) {
		san += "x";
	}
	// add the destination square
	san += util::squareStrings[(bits & toMask) >> toShift];

	// add the promotion
	if ((bits & promoMask) >> promoShift == 1) {
		// get piece promoting to
		san += util::pieceAbbr[(bits & utilMask) >> utilShift];
	}

	return san;
}