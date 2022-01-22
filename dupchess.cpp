#include <iostream>
#include <bitset>
#include "include/global.h"

using namespace std;

int main()
{
	std::string FEN;
	BOARD_S bb = BOARD_S();



	FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	setBoardFromFEN(bb, FEN);

	printf("\nAll Pieces");
	printbitboard(bb.white_pcs | bb.black_pcs);

	return 0;
}

