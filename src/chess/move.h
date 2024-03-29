#pragma once

#include "utils.h"

// This header is for a class that encodes a single piece move
class Move {
public:
	// constructor
	Move();
	Move(uint32_t data);
	Move(uint32_t fromInd, uint32_t toInd, util::Piece pID, bool isCap, bool isPromo, bool isEP, bool isCastle, uint8_t util_bits);

	// return long SAN notation string for move
	std::string getLongAN();

	// get information about the move
	int getFromSquare();
	int getToSquare();
	util::Piece getPieceID();
	bool isCapture();
	bool isPawnPromo();
	bool isEnPassant();
	bool isCastle();
	uint8_t getUtilValue();

	// change piece ID (used for queen move generation, may be a dumb thing to do i dont know)
	void setPieceID(util::Piece newID);

	// set move score (eval in centipawns)
	void setScore(int eval);

	// get eval for this move
	int getEvalCP();

	// compare moves by eval score
	bool operator<(const Move& m);

private:
	uint32_t bits; // encoded move
	int score{};   // holds score after nn evaluation

	// bit shifts
	// bits 0-5 = index of current piece square (0 to 63), no shift
	static const int toShift = 6;			// bits 6-11 = index of square to move to (0 to 63)
	static const int pieceShift = 12;		// bits 12-14 = piece id (000 = pawn, 001 = bishop, 010 = knight, 011 = rook, 100 = queen, 101 = king)
	static const int capShift = 15;			// bit 15, 1 if capture, 0 otherwise
	static const int promoShift = 16;		// bit 16, 1 if promotion (see utility bits for piece to promote into (001 = bishop, 010 = knight, 011 = rook, 100 = queen)
	static const int epShift = 17;			// bit 17, 1 if an en passant square is revealed (square index in util bits) OR an en passant capture move (if capture bit is also set)
	static const int utilShift = 18;		// bits 18-25, utility for encoding promo etc
	static const int checkShift = 26;		// bit 26, set if move is a checking move (other king is in check)
	static const int castleShift = 27;		// bit 27, set if move is a castle (util bits will tell if it is king or queenside)


	// bit masks
	static const int fromMask = 0x3F;				// bits 0-5 = index of current piece
	static const int toMask = 0xFC0;				// bits 6-11 = index of square to move to (0 to 63)
	static const int pieceMask = 0x7000;			// bits 12-14 = piece id
	static const int capMask = 0x8000;				// bit 15, 1 if capture 0 otherwise
	static const int promoMask = 0x10000;			// bit 16, 1 if promotion 0 otherwise
	static const int epMask = 0x20000;				// bit 17, en passant
	static const int utilMask = 0xFF0000;			// bit 18-25, util bits
	static const int checkMask = 1 << checkShift;	// bit 26, is move a checking move
	static const int castleMask = 1 << castleShift; // bit 27, is castle move
};