
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

	uint64_t piece_index = 63;
	for (int ii = 0; ii < splitstr[0].length(); ii++) {
		char c_char = splitstr[0][ii];
		if (isalpha(c_char)) // piece
		{
			if (isupper(c_char)) // white piece
			{
				(*this).current_state.white_pcs = (1ULL << piece_index) | (*this).current_state.white_pcs;
			}
			else // black piece
			{
				(*this).current_state.black_pcs = (1ULL << piece_index) | (*this).current_state.black_pcs;
			}

			// piece type
			switch (tolower(c_char)) {
			case 'r':
				(*this).current_state.rooks = (1ULL << piece_index) | (*this).current_state.rooks;
				break;
			case 'n':
				(*this).current_state.knights = (1ULL << piece_index) | (*this).current_state.knights;
				break;
			case 'b':
				(*this).current_state.bishops = (1ULL << piece_index) | (*this).current_state.bishops;
				break;
			case 'q':
				(*this).current_state.queens = (1ULL << piece_index) | (*this).current_state.queens;
				break;
			case 'k':
				(*this).current_state.kings = (1ULL << piece_index) | (*this).current_state.kings;
				break;
			case 'p':
				(*this).current_state.pawns = (1ULL << piece_index) | (*this).current_state.pawns;
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
		(*this).current_state.whiteToMove = true;
	else
		(*this).current_state.whiteToMove = false;

	// castling rights from third part of FEN
	(*this).current_state.castleRights = 0x0;
	if (splitstr[2] != "-")  // need to set one or more bits
	{
		for (auto& ch : splitstr[2])
		{
			switch (ch)
			{
			case 'K':
				(*this).current_state.castleRights = (*this).current_state.castleRights | std::bitset<4>(0x8);
				break;
			case 'Q':
				(*this).current_state.castleRights = (*this).current_state.castleRights | std::bitset<4>(0x4);
				break;
			case 'k':
				(*this).current_state.castleRights = (*this).current_state.castleRights | std::bitset<4>(0x2);
				break;
			case 'q':
				(*this).current_state.castleRights = (*this).current_state.castleRights | std::bitset<4>(0x1);
				break;
			}
		}
	}

	// en passant target square from fourth part of FEN
	if (splitstr[3] == "=")
		(*this).current_state.PIPI = squares::EMPTY_SQ;
	else
	{
		(*this).current_state.PIPI = stringToSquare(splitstr[3]);
	}

	// half move/full move counters
	(*this).current_state.halfmove = stoi(splitstr[4]);
	(*this).current_state.fullmove = stoi(splitstr[5]);

}

// clear all bitboards (used prior to piece assignment from FEN)
void Board::clearBoard() {
	(*this).current_state.white_pcs = 0ULL;		// all white pieces
	(*this).current_state.black_pcs = 0ULL;		// all black pieces
	(*this).current_state.pawns = 0ULL;			// all pawns
	(*this).current_state.rooks = 0ULL;			// all rooks
	(*this).current_state.knights = 0ULL;		// all knights
	(*this).current_state.bishops = 0ULL;		// all bishops
	(*this).current_state.queens = 0ULL; 		// all queens
	(*this).current_state.kings = 0ULL;			// all kings
	}