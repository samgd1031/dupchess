#include <iostream>
#include <bitset>
#include "include/global.h"

using namespace std;

int main()
{
	std::string FEN;
	BOARD_S bb = BOARD_S();

	FEN = "K2b2q1/8/2Q5/6pk/P7/7Q/8/8 b - b5 2 67";
	setBoardFromFEN(bb, FEN);

	printf("\nAll Pieces");
	printbitboard(bb.white_pcs | bb.black_pcs);

	printf("\nWhite Rooks");
	printbitboard(bb.white_pcs & bb.rooks);

	printf("\nBishops");
	printbitboard(bb.bishops);

	cout << "Castling Rights (KQkq): " << bb.castleRights << endl;
	cout << "En Passant Target: " << squareStrings[int(bb.PIPI)] << endl;

	return 0;
}

