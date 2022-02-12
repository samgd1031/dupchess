#pragma once

#include "utils.h"

// This header is for a class that encodes a single piece move
class Move {
public:
	// constructor
	Move(uint32_t data);
	Move(uint32_t fromInd, uint32_t toInd, bool isCap, bool isPromo);

	// return long SAN notation string for move
	std::string getLongSAN();

private:
	uint32_t bits; // encoded move

	// bit shifts
	// bits 0-5 = index of current piece square (0 to 63), no shift
	int toShift = 6;		// bits 6-11 = index of square to move to (0 to 63)
	int pieceShift = 12;	// bits 12-14 = piece id (000 = pawn, 001 = bishop, 010 = knight, 011 = rook, 100 = queen, 101 = king)
	int capShift = 15;		// bit 15, 1 if capture, 0 otherwise
	int promoShift = 16;	// bit 16, 1 if promotion (see utility bits for piece to promote into (001 = bishop, 010 = knight, 011 = rook, 100 = queen)
	int utilShift = 17;		// bits 17-19, utility for encoding promo etc


	// bit masks
	int fromMask = 0x3F;		// bits 0-5 = index of current piece
	int toMask = 0xFC0;			// bits 6-11 = index of square to move to (0 to 63)
	int pieceMask = 0x7000;		// bits 12-14 = piece id
	int capMask = 0x8000;		// bit 15, 1 if capture 0 otherwise
	int promoMask = 0x10000;	// bit 16, 1 if promotion 0 otherwise
	int utilMask = 0xE0000;		// bit 17-19, util bits
};