
#include "board.h"


// sets board state from FEN string
// currently does minimal error checking, assumes it will be given a valid FEN
void Board::setBoardFromFEN(std::string FENstring) {

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
	(*this).clearBoard();
	//

	uint64_t piece_index = 56;
	for (int ii = 0; ii < splitstr[0].length(); ii++) {
		char c_char = splitstr[0][ii];
		if (isalpha(c_char)) // piece
		{
			if (isupper(c_char)) // white piece
			{
				(*this).state.white_pcs = (1ULL << piece_index) | (*this).state.white_pcs;
			}
			else // black piece
			{
				(*this).state.black_pcs = (1ULL << piece_index) | (*this).state.black_pcs;
			}

			// piece type
			switch (tolower(c_char)) {
			case 'r':
				(*this).state.rooks = (1ULL << piece_index) | (*this).state.rooks;
				break;
			case 'n':
				(*this).state.knights = (1ULL << piece_index) | (*this).state.knights;
				break;
			case 'b':
				(*this).state.bishops = (1ULL << piece_index) | (*this).state.bishops;
				break;
			case 'q':
				(*this).state.queens = (1ULL << piece_index) | (*this).state.queens;
				break;
			case 'k':
				(*this).state.kings = (1ULL << piece_index) | (*this).state.kings;
				break;
			case 'p':
				(*this).state.pawns = (1ULL << piece_index) | (*this).state.pawns;
				break;
			}

			piece_index++;
		}
		else if (isdigit(c_char)) // if digit, increment number of empty squares
		{
			piece_index += ((uint64_t)c_char - '0');
		}
		else if (c_char == '/') {// go down to the next rank
			piece_index -= 16;
		}
	}

	// set active color from second part of FEN
	if (splitstr[1] == "w")
		(*this).state.whiteToMove = true;
	else
		(*this).state.whiteToMove = false;

	// castling rights from third part of FEN
	(*this).state.castleRights = 0x0;
	if (splitstr[2] != "-")  // need to set one or more bits
	{
		for (auto& ch : splitstr[2])
		{
			switch (ch)
			{
			case 'K':
				(*this).state.castleRights = (*this).state.castleRights | std::bitset<4>(0x8);
				break;
			case 'Q':
				(*this).state.castleRights = (*this).state.castleRights | std::bitset<4>(0x4);
				break;
			case 'k':
				(*this).state.castleRights = (*this).state.castleRights | std::bitset<4>(0x2);
				break;
			case 'q':
				(*this).state.castleRights = (*this).state.castleRights | std::bitset<4>(0x1);
				break;
			}
		}
	}

	// en passant target square from fourth part of FEN
	if (splitstr[3] == "-")
		(*this).state.PIPI = util::squares::EMPTY_SQ;
	else
	{
		(*this).state.PIPI = util::stringToSquare(splitstr[3]);
	}

	// half move/full move counters
	(*this).state.halfmove = stoi(splitstr[4]);
	(*this).state.fullmove = stoi(splitstr[5]);

}

// clear all bitboards (used prior to piece assignment from FEN)
void Board::clearBoard() {
	(*this).state.white_pcs = 0ULL;		// all white pieces
	(*this).state.black_pcs = 0ULL;		// all black pieces
	(*this).state.pawns = 0ULL;			// all pawns
	(*this).state.rooks = 0ULL;			// all rooks
	(*this).state.knights = 0ULL;		// all knights
	(*this).state.bishops = 0ULL;		// all bishops
	(*this).state.queens = 0ULL; 		// all queens
	(*this).state.kings = 0ULL;			// all kings
	}