#pragma once

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

const char* squareStrings[65]{
	"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
	"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
	"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
	"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
	"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
	"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
	"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
	"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8", "EMPTY_SQ"
};

// helper function to convert a string with a square name to the squares enum
squares stringToSquare(std::string squareString)
{
	try
	{
		if (squareString.length() != 2)
			return squares::EMPTY_SQ;

		char file = toupper(squareString[0]);
		char rank = squareString[1];

		int filenum = file - 'A';
		int ranknum = rank - '1';

		return squares(filenum + 8*ranknum);
	}
	catch(...)
	{
		return squares::EMPTY_SQ;
	}
}


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

