
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
	for (int rank = 8; rank > 0; rank--) {
		printf("\n%i ", rank);
		for (int file = (rank - 1) * 8; file < rank * 8; file++) {
			if (bb & (1ULL << file))
				printf("1");
			else
				printf("0");
		}
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

// hyperbola quintessence for sliding attacks
bitboard util::hyp_quint(bitboard occ, bitboard mask, int sqInd) {
	bitboard o, r, o_rev, r_rev;
	o = occ & mask;
	r = 1ULL << sqInd;
	o_rev = byteswap_u64(o);
	r_rev = byteswap_u64(r);


	return ((o - (2 * r)) ^ byteswap_u64(o_rev - (2 * r_rev))) & mask;
}

// get valid rook moves/attacks along a rank (horizontally)
bitboard util::rankAttacks(bitboard occ, int sqInd){
	int rank = sqInd / 8;
	bitboard rank_o = (occ & util::rankMasks[rank]);

	// extract rank with sliding piece
	uint8_t occ_byte = rank_o >> rank*8;
	int file = sqInd % 8;

	bitboard rank_atk = 0;

	// moves/attacks towards H file
	int ii = file + 1;
	while (ii < 8) {
		rank_atk |= 1ULL << ii;
		// if bit is occupied, set this as a valid move (capture) and break
		if ((occ_byte >> ii) & 1U) {break;}
		ii++;
	}
	// moves/attacks towards A file
	ii = file - 1;
	while (ii > -1) {
		rank_atk |= 1ULL << ii;
		// if bit is occupied, set this as a valid move (capture) and break
		if ((occ_byte >> ii) & 1U) { break; }
		ii--;
	}

	// shift back to appropriate rank and return
	return rank_atk << rank * 8;

}