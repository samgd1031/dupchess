#include "move.h"


// Constructor
Move::Move() {
	bits = 0;
}

Move::Move(uint32_t data) {
	bits = data;
}

Move::Move(uint32_t fromInd, uint32_t toInd, util::Piece pID, bool isCap, bool isPromo, bool isEP, bool isCastle, uint8_t util_bits) {
	bits = 0;
	bits = fromInd;
	bits |= toInd << toShift;
	switch (pID) {
	case util::Piece::PAWN:
		bits |= 0x00000000 << pieceShift;
		break;
	case util::Piece::BISHOP:
		bits |= 0x00000001 << pieceShift;
		break;
	case util::Piece::KNIGHT:
		bits |= 0x00000002 << pieceShift;
		break;
	case util::Piece::ROOK:
		bits |= 0x00000003 << pieceShift;
		break;
	case util::Piece::QUEEN:
		bits |= 0x00000004 << pieceShift;
		break;
	case util::Piece::KING:
		bits |= 0x00000005 << pieceShift;
		break;
	default:
		break;
	}
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

	if (isCastle) bits |= castleMask;
}


// return a long algebraic string for the move (from square, to square, piece if promo)
std::string Move::getLongAN() {
	std::string buf(util::squareStrings[getFromSquare()]);
	buf.append(util::squareStrings[getToSquare()]);

	// add the promotion piece if necessary
	if ((bits & promoMask) >> promoShift == 1) {
		// get piece promoting to
		buf += util::pieceAbbr[(bits & utilMask) >> utilShift];
	}

	return buf;
}

// return index of from square
int Move::getFromSquare() { return (bits & fromMask); }

// return index of to square
int Move::getToSquare() { return (bits & toMask) >> toShift; }

// return piece ID
util::Piece Move::getPieceID() { return (util::Piece)((bits& pieceMask) >> pieceShift); }

// return 1 if this move is a capture, 0 otherwise
bool Move::isCapture() { return (bits & capMask) >> capShift; }

// return 1 if this move is a pawn promotion, 0 otherwise
bool Move::isPawnPromo() { return (bits & promoMask) >> promoShift; }

// return 1 if this move is a pawn promotion, 0 otherwise
bool Move::isEnPassant() { return (bits & epMask) >> epShift; }

// return true if move is a castle
bool Move::isCastle() { return (bits & castleMask) >> castleShift; }

// return value in utility bits
uint8_t Move::getUtilValue() { return (bits & utilMask) >> utilShift; }

// change piece ID (used for queen move generation, may be a dumb thing to do i dont know)
void Move::setPieceID(util::Piece newID) {
	// clear current pieceID
	bits &= ~(pieceMask);
	switch (newID) {
	case util::Piece::PAWN:
		bits |= 0x00000000 << pieceShift;
		break;
	case util::Piece::BISHOP:
		bits |= 0x00000001 << pieceShift;
		break;
	case util::Piece::KNIGHT:
		bits |= 0x00000002 << pieceShift;
		break;
	case util::Piece::ROOK:
		bits |= 0x00000003 << pieceShift;
		break;
	case util::Piece::QUEEN:
		bits |= 0x00000004 << pieceShift;
		break;
	case util::Piece::KING:
		bits |= 0x00000005 << pieceShift;
		break;
	}
}
