#pragma once

#include "utils.h"

// This header is for a class that encodes a single piece move
class Move {
public:
	// constructor
	Move(uint32_t data);
	Move(uint32_t fromInd, uint32_t toInd, bool isCap, bool isPromo, bool isEP, uint8_t util_bits);

	// return long SAN notation string for move
	std::string getLongSAN();

	// get information about the move
	int getFromSquare();
	int getToSquare();
	int getPieceID();
	int isCapture();
	int isPawnPromo();
	int isDoublePawnPush();
	uint8_t getUtilValue();

private:
	uint32_t bits; // encoded move

	// bit shifts
	// bits 0-5 = index of current piece square (0 to 63), no shift
	static const int toShift = 6;			// bits 6-11 = index of square to move to (0 to 63)
	static const int pieceShift = 12;		// bits 12-14 = piece id (000 = pawn, 001 = bishop, 010 = knight, 011 = rook, 100 = queen, 101 = king)
	static const int capShift = 15;			// bit 15, 1 if capture, 0 otherwise
	static const int promoShift = 16;		// bit 16, 1 if promotion (see utility bits for piece to promote into (001 = bishop, 010 = knight, 011 = rook, 100 = queen)
	static const int epShift = 17;			// bit 17, 1 if an en passant square is revealed (square index in util bits)
	static const int utilShift = 18;		// bits 18-25, utility for encoding promo etc


	// bit masks
	static const int fromMask = 0x3F;			// bits 0-5 = index of current piece
	static const int toMask = 0xFC0;			// bits 6-11 = index of square to move to (0 to 63)
	static const int pieceMask = 0x7000;		// bits 12-14 = piece id
	static const int capMask = 0x8000;			// bit 15, 1 if capture 0 otherwise
	static const int promoMask = 0x10000;		// bit 16, 1 if promotion 0 otherwise
	static const int epMask = 0x20000;			// bit 17, en passant
	static const int utilMask = 0xFF0000;		// bit 18-25, util bits
};