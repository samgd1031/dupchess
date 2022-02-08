#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <ctype.h>
#include <bitset>

typedef uint64_t bitboard;

enum class colors {WHITE, BLACK, NONE};

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

extern char* squareStrings[65];

// struct to represent the state of the game at any given time
struct BOARD_STATE {
	// piece type of a color can be found by ANDing white/black_pcs with
	// a piece type bitboard
	// values below represent a new board/no moves made
	bitboard white_pcs = 0x000000000000FFFF;		// all white pieces
	bitboard black_pcs = 0xFFFF000000000000;		// all black pieces
	bitboard pawns = 0x00FF00000000FF00;			// all pawns
	bitboard rooks = 0x8100000000000081;			// all rooks
	bitboard knights = 0x4200000000000042;			// all knights
	bitboard bishops = 0x2400000000000024;			// all bishops
	bitboard queens = 0x1000000000000010; 			// all queens
	bitboard kings = 0x0800000000000008;			// all kings

	// other game state info
	squares PIPI = squares::EMPTY_SQ;	// en passant
	bool whiteToMove = true;			// who gets to move
	std::bitset<4> castleRights{ 0xF };	// 4 bits from high to low: white kingside, white queenside, blk kingside, blk queenside

	int halfmove = 0;  // half move counter
	int fullmove = 1;  // full move counter

};

// helper function to convert a string with a square name to the squares enum
squares stringToSquare(std::string squareString);

// function to print a bitboard to console for debugging
void printbitboard(bitboard bb);

