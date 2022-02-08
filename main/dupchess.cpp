
#include <iostream>
#include <bitset>

#include "board.h"


int main()
{
	std::string FEN;
	board::BOARD_S bb = board::BOARD_S();


	FEN = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2 ";
	board::setBoardFromFEN(bb, FEN);

	printf("\nAll Pieces");
	printbitboard(bb.white_pcs | bb.black_pcs);

	printf("\nWhite Rooks");
	printbitboard(bb.white_pcs & bb.rooks);

	printf("\nBishops");
	printbitboard(bb.bishops);

	std::cout << "Castling Rights (KQkq): " << bb.castleRights << std::endl;
	std::cout << "En Passant Target: " << squareStrings[int(bb.PIPI)] << std::endl;
	std::cout << "To Move: " << ((bb.whiteToMove) ? "White" : "Black") << std::endl;
	std::cout << "Half Move Counter: " << bb.halfmove << std::endl;
	std::cout << "Full Move Counter: " << bb.fullmove << std::endl;

	return 0;
}

