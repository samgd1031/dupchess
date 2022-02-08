
#include <bitset>
#include "global.h"

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

		return squares(filenum + 8 * ranknum);
	}
	catch (...)
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