#include <iostream>
#include <bitset>
#include "include/global.h"

using namespace std;

int main()
{
	std::string FEN;
	BOARD_S bb = BOARD_S();

	FEN = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2 ";
	setBoardFromFEN(bb, FEN);

	printf("\nAll Pieces");
	printbitboard(bb.white_pcs | bb.black_pcs);

	printf("\nWhite Rooks");
	printbitboard(bb.white_pcs & bb.rooks);

	printf("\nBishops");
	printbitboard(bb.bishops);

	cout << "Castling Rights (KQkq): " << bb.castleRights << endl;
	cout << "En Passant Target: " << squareStrings[int(bb.PIPI)] << endl;
	cout << "Half Move Counter: " << bb.halfmove << endl;
	cout << "Full Move Counter: " << bb.fullmove << endl;

	return 0;
}

