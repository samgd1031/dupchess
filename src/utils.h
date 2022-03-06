#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <ctype.h>
#include <bitset>

typedef uint64_t bitboard;

namespace util {
	enum class colors { WHITE, BLACK, NONE };

	enum class Piece { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING, NONE };

	static const char* pieceAbbr[6]{ "P","B","N","R","Q","K" };

	enum class squares {
		A1, B1, C1, D1, E1, F1, G1, H1,
		A2, B2, C2, D2, E2, F2, G2, H2,
		A3, B3, C3, D3, E3, F3, G3, H3,
		A4, B4, C4, D4, E4, F4, G4, H4,
		A5, B5, C5, D5, E5, F5, G5, H5,
		A6, B6, C6, D6, E6, F6, G6, H6,
		A7, B7, C7, D7, E7, F7, G7, H7,
		A8, B8, C8, D8, E8, F8, G8, H8,
		EMPTY_SQ
	};

	// index zero is a mask for rank 1, 7 for rank 8
	static const bitboard rankMasks[8]{
		0x00000000000000FF,
		0x000000000000FF00,
		0x0000000000FF0000,
		0x00000000FF000000,
		0x000000FF00000000,
		0x0000FF0000000000,
		0x00FF000000000000,
		0xFF00000000000000,
	};

	// index 0 is a mask for file a, 7 for file h
	static const bitboard fileMasks[8]{
		0x0101010101010101,
		0x0202020202020202,
		0x0404040404040404,
		0x0808080808080808,
		0x1010101010101010,
		0x2020202020202020,
		0x4040404040404040,
		0x8080808080808080

	};

	// diagonal mask, index by (rank - file) & 15
	static const bitboard diagMasks[16]{
		0x8040201008040201,
		0x4020100804020100,
		0x2010080402010000,
		0x1008040201000000,
		0x804020100000000,
		0x402010000000000,
		0x201000000000000,
		0x100000000000000,
		0x0,
		0x80,
		0x8040,
		0x804020,
		0x80402010,
		0x8040201008,
		0x804020100804,
		0x80402010080402
	};

	// antidiagonal mask, index by (rank + file) ^ 7
	static const bitboard adiagMasks[16]{
		0x102040810204080,
		0x1020408102040,
		0x10204081020,
		0x102040810,
		0x1020408,
		0x10204,
		0x102,
		0x1,
		0x0,
		0x8000000000000000,
		0x4080000000000000,
		0x2040800000000000,
		0x1020408000000000,
		0x810204080000000,
		0x408102040800000,
		0x204081020408000
	};

	// mask for king attack (centered around B2 square, can be shifted)
	static const bitboard kingAttack = 0x0000000000070507;
	static const int kingAttackIndex = 9; // reference index to shift by


	static const char* squareStrings[65]{
		"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
		"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
		"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
		"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
		"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
		"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
		"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
		"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
		"EMPTY_SQ"
	};


	// struct to represent the state of the game at any given time
	struct BOARD_STATE {
		// piece type of a color can be found by ANDing white/black_pcs with
		// a piece type bitboard
		// values below represent a blank board
		bitboard white_pcs = 0ULL;		// all white pieces
		bitboard black_pcs = 0ULL;		// all black pieces
		bitboard pawns = 0ULL;			// all pawns
		bitboard rooks = 0ULL;			// all rooks
		bitboard knights = 0ULL;		// all knights
		bitboard bishops = 0ULL;		// all bishops
		bitboard queens = 0ULL; 		// all queens
		bitboard kings = 0ULL;			// all kings

		// other game state info
		util::squares PIPI = squares::EMPTY_SQ;	// en passant
		bool whiteToMove = true;			// who gets to move
		std::bitset<4> castleRights{ 0xF };	// 4 bits from high to low: white kingside, white queenside, blk kingside, blk queenside

		int halfmove = 0;  // half move counter
		int fullmove = 1;  // full move counter

	};

	// helper function to convert a string with a square name to the squares enum
	squares stringToSquare(std::string squareString);

	// function to print a bitboard to console for debugging
	void printbitboard(bitboard bb);

	// function to flip a bitboard
	bitboard flipBitboard(bitboard bb);

	// generalized bit shift (left for positive, right for negative)
	uint64_t genShift(uint64_t x, int s);

	// hyperbola quintessence for sliding attacks
	bitboard hyp_quint(bitboard occ, bitboard mask, int sqInd);

	// get valid rook occupancy on a rank
	bitboard rankAttacks(bitboard occ, int sqInd);
};