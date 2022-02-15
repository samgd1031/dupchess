#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <ctype.h>
#include <bitset>

typedef uint64_t bitboard;

namespace util {
	enum class colors { WHITE, BLACK, NONE };

	static const char* pieceAbbr[6]{ "P","B","N","R","Q","K" };

	enum class squares {
		H1, G1, F1, E1, D1, C1, B1, A1,
		H2, G2, F2, E2, D2, C2, B2, A2,
		H3, G3, F3, E3, D3, C3, B3, A3,
		H4, G4, F4, E4, D4, C4, B4, A4,
		H5, G5, F5, E5, D5, C5, B5, A5,
		H6, G6, F6, E6, D6, C6, B6, A6,
		H7, G7, F7, E7, D7, C7, B7, A7,
		H8, G8, F8, E8, D8, C8, B8, A8,
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
		0x8080808080808080,
		0x4040404040404040,
		0x2020202020202020,
		0x1010101010101010,
		0x0808080808080808,
		0x0404040404040404,
		0x0202020202020202,
		0x0101010101010101,

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