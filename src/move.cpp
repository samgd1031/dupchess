#include "move.h"


// Constructor
Move::Move(uint32_t data) {
	bits = data;
}

Move::Move(uint32_t fromInd, uint32_t toInd, bool isCap, bool isPromo, bool isEP, uint8_t util_bits) {
	bits = 0;
	bits = fromInd;
	bits |= toInd << toShift;
	if (isCap) bits |= capMask;
	if (isPromo) bits |= promoMask;
	if (isEP) {
		bits |= epMask;						// set bit to encode this as an en passant move
		// set util bits to have index of enpassant square
		if (toInd > fromInd) {
			bits |= (toInd - 8) << utilShift;	// set util bits to have index of enpassant square
		}
		else {
			bits |= (toInd + 8) << utilShift;
		}
	}
	bits |= util_bits << utilShift;
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

// return index of from square
int Move::getFromSquare() { return (bits & fromMask); }

// return index of to square
int Move::getToSquare() { return (bits & toMask) >> toShift; }

// return piece ID
int Move::getPieceID() { return (bits & pieceMask) >> pieceShift; }

// return 1 if this move is a capture, 0 otherwise
int Move::isCapture() { return (bits & capMask) >> capShift; }

// return 1 if this move is a pawn promotion, 0 otherwise
int Move::isPawnPromo() { return (bits & promoMask) >> promoShift; }

// return 1 if this move is a double pawn push (and therefore en passant is possible), 0 otherwise
int Move::isDoublePawnPush() { return (bits & epMask) >> epShift; }

// return value in utility bits
uint8_t Move::getUtilValue() { return (bits & utilMask) >> utilShift; }
