#ifndef GL_H
#define GL_H

#include <cstdint>
typedef uint64_t bitboard;

#define NAME "DupChess"

enum pieces	{EM, wP, wR, wN, wB, wQ, wK, bP, bR, bN, bB, bQ, bK, };
enum colors	{WHITE, BLACK, NONE};
enum squares {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8, EMPTY_SQ
};

typedef struct {
	// piece type of a color can be found by unioning white/black_pcs with
	// a piece type bitboard
	bitboard white_pcs;		// all white pieces
	bitboard black_pcs;		// all black pieces
	bitboard pawns;			// all pawns
	bitboard rooks;			// all rooks
	bitboard knights;		// all knights
	bitboard bishops;		// all bishops
	bitboard queens;		// all queens
	bitboard kings;			// all kings

} BOARD_S;

#endif // !GL_H


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
	printf("\n  ABCDEFGH");
}