#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <ctype.h>
#include <bitset>

typedef uint64_t bitboard;

namespace util {
	enum class colors { WHITE, BLACK, NONE };

	static const char* pieceAbbr[6]{ "p","B","N","R","Q","K" };

	enum class squares {
		A1, B1, C1, D1, E1, F1, G1, H1,
		A2, B2, C2, D2, E2, F2, G2, H2,
		A3, B3, C3, D3, E3, F3, G3, H3,
		A4, B4, C4, D4, E4, F4, G4, H4,
		A5, B5, C5, D5, E5, F5, G5, H5,
		A6, B6, C6, D6, E6, F6, G6, H6,
		A7, B7, C7, D7, E7, F7, G7, H7,
		A8, B8, C8, D8, E8, F8, G8, H8, EMPTY_SQ
	};


	static const char* squareStrings[65]{
		"h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1",
		"h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2",
		"h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3",
		"h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4",
		"h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5",
		"h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6",
		"h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7",
		"h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8",
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
};