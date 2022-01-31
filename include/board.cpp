#pragma once

#include "board.h"
#include <vector>
#include <string>

// sets board state from FEN string
// currently does minimal error checking, assumes it will be given a valid FEN
void board::setBoardFromFEN(board::BOARD_S& bb, std::string FENstring) {

	std::string tmp = "";
	std::vector<std::string> splitstr;

	// first, split up FEN string at spaces
	for (int ii = 0; ii < FENstring.length(); ii++) {
		if (FENstring[ii] == ' ') {
			splitstr.push_back(tmp);
			tmp = "";
		}
		else {
			tmp.push_back(FENstring[ii]);
		}
	}
	splitstr.push_back(tmp);  // add last part of FEN string to splitstr

	// set piece locations from first part of FEN
	board::clearBoard(bb);
	//

	uint64_t piece_index = 63;
	for (int ii = 0; ii < splitstr[0].length(); ii++) {
		char c_char = splitstr[0][ii];
		if (isalpha(c_char)) // piece
		{
			if (isupper(c_char)) // white piece
			{
				bb.white_pcs = (1ULL << piece_index) | bb.white_pcs;
			}
			else // black piece
			{
				bb.black_pcs = (1ULL << piece_index) | bb.black_pcs;
			}

			// piece type
			switch (tolower(c_char)) {
			case 'r':
				bb.rooks = (1ULL << piece_index) | bb.rooks;
				break;
			case 'n':
				bb.knights = (1ULL << piece_index) | bb.knights;
				break;
			case 'b':
				bb.bishops = (1ULL << piece_index) | bb.bishops;
				break;
			case 'q':
				bb.queens = (1ULL << piece_index) | bb.queens;
				break;
			case 'k':
				bb.kings = (1ULL << piece_index) | bb.kings;
				break;
			case 'p':
				bb.pawns = (1ULL << piece_index) | bb.pawns;
				break;
			}

			piece_index--;
		}
		else if (isdigit(c_char)) // if digit, increment number of empty squares
		{
			piece_index -= ((uint64_t)c_char - '0');
		}
	}

	// set active color from second part of FEN
	if (splitstr[1] == "w")
		bb.whiteToMove = true;
	else
		bb.whiteToMove = false;

	// castling rights from third part of FEN
	bb.castleRights = 0x0;
	if (splitstr[2] != "-")  // need to set one or more bits
	{
		for (auto& ch : splitstr[2])
		{
			switch (ch)
			{
			case 'K':
				bb.castleRights = bb.castleRights | std::bitset<4>(0x8);
				break;
			case 'Q':
				bb.castleRights = bb.castleRights | std::bitset<4>(0x4);
				break;
			case 'k':
				bb.castleRights = bb.castleRights | std::bitset<4>(0x2);
				break;
			case 'q':
				bb.castleRights = bb.castleRights | std::bitset<4>(0x1);
				break;
			}
		}
	}

	// en passant target square from fourth part of FEN
	if (splitstr[3] == "=")
		bb.PIPI = squares::EMPTY_SQ;
	else
	{
		bb.PIPI = stringToSquare(splitstr[3]);
	}

	// half move/full move counters
	bb.halfmove = stoi(splitstr[4]);
	bb.fullmove = stoi(splitstr[5]);

}

// clear all bitboards (used prior to piece assignment from FEN)
static void board::clearBoard(board::BOARD_S& bb) {
	bb.white_pcs = 0x0000000000000000;		// all white pieces
	bb.black_pcs = 0x0000000000000000;		// all black pieces
	bb.pawns = 0x0000000000000000;			// all pawns
	bb.rooks = 0x0000000000000000;			// all rooks
	bb.knights = 0x0000000000000000;		// all knights
	bb.bishops = 0x0000000000000000;		// all bishops
	bb.queens = 0x0000000000000000; 		// all queens
	bb.kings = 0x0000000000000000;			// all kings
}