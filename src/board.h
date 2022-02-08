#pragma once

#include "global.h"
#include <bitset>

namespace board {
	// struct that represents the board state at any given turn
	struct BOARD_S {
		// piece type of a color can be found by ANDing white/black_pcs with
		// a piece type bitboard
		// initializes to a new board/no moves made
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

	// sets board state from FEN string
	void setBoardFromFEN(BOARD_S& bb, std::string FENstring);
	void clearBoard(board::BOARD_S& bb);
}