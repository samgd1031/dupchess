
#include <bitset>
#include "utils.h"

// helper function to convert a string with a square name to the squares enum
util::squares util::stringToSquare(std::string squareString)
{
	try
	{
		if (squareString.length() != 2)
			return util::squares::EMPTY_SQ;

		char file = toupper(squareString[0]);
		char rank = squareString[1];

		int filenum = file - 'A';
		int ranknum = rank - '1';

		return util::squares(filenum + 8 * ranknum);
	}
	catch (...)
	{
		return util::squares::EMPTY_SQ;
	}
}

// function to print a bitboard to console for debugging
void util::printbitboard(bitboard bb) {
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

// function to flip a bitboard
bitboard util::flipBitboard(bitboard bb) {
	uint64_t flipped = 0ULL;
	unsigned int s = sizeof(bb) * CHAR_BIT; // bit size; must be power of 2 
	for (int ii = s-1; ii >= 0; ii--) {
		flipped |= (bb & 1ULL) << ii;
		bb >>= 1ULL;
	}
	return flipped;
}

// generalized bit shift (left for positive, right for negative)
uint64_t util::genShift(uint64_t x, int s) {
	return (s > 0) ? (x << s) : (x >> -s);
}