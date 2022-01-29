#ifndef GL_H
#define GL_H

#include <cstdint>
#include <string>
#include <vector>
#include <ctype.h>

typedef uint64_t bitboard;

#define NAME "DupChess"

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

typedef struct BOARD_S {
	// piece type of a color can be found by unioning white/black_pcs with
	// a piece type bitboard
	bitboard white_pcs = 0x0000000000000000;		// all white pieces
	bitboard black_pcs = 0x0000000000000000;		// all black pieces
	bitboard pawns = 0x0000000000000000;			// all pawns
	bitboard rooks = 0x0000000000000000;			// all rooks
	bitboard knights = 0x0000000000000000;			// all knights
	bitboard bishops = 0x0000000000000000;			// all bishops
	bitboard queens = 0x0000000000000000;			// all queens
	bitboard kings = 0x0000000000000000;			// all kings

	// other game state info
	squares PIPI = squares::EMPTY_SQ; // en passant
	bool whiteToMove = true;  // who gets to move
	std::bitset<4> castleRights{0xF};  // last 4 bits of uint8, from high to low: white kingside, white queenside, blk kingside, blk queenside

	int halfmove = 0;  // half move clock
	int fullmove = 0;  // full move counter

};



// function to print a bitboard to console for debugging
void printbitboard(bitboard bb) {
	uint64_t ii;
	uint64_t count;

	ii = 1ULL << (sizeof(bb) * CHAR_BIT - 1);
	count = 64;
	while (ii > 0) {
		if (count % 8 == 0)
			printf("\n%llu ", count / 8);

		if (bb & ii)
			printf("1");
		else
			printf("0");
		ii >>= 1;
		count--;

	}
	printf("\n  ABCDEFGH\n\n");
}

void setBoardFromFEN(BOARD_S &bb, std::string FENstring) {
	
	std::string tmp = "";
	std::vector<std::string> splitstr;

	// first, split up FEN string at spaces
	for (int ii = 0; ii < FENstring.length(); ii++) {
		if (FENstring[ii] == ' '){
			splitstr.push_back(tmp);
			tmp = "";
		}
		else {
			tmp.push_back(FENstring[ii]);
		}
	}

	// set piece locations from first part of FEN
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
			piece_index -= (uint64_t)(c_char - '0');
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
		std::cout << std::endl;
	}

}


#endif // !GL_H