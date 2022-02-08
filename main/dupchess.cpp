
#include <iostream>
#include <bitset>

#include "board.h"


int main()
{
	std::string FEN;
	Board gameboard = Board();


	FEN = "rn2k3/pp3p1p/5b2/3p1b1p/2P5/1P6/PB1P4/2K2q2 w q - 6 28";
	gameboard.setBoardFromFEN(FEN);

	printf("\nAll Pieces");
	printbitboard(gameboard.current_state.white_pcs | gameboard.current_state.black_pcs);

	printf("\nWhite Rooks");
	printbitboard(gameboard.current_state.white_pcs & gameboard.current_state.rooks);

	printf("\nBishops");
	printbitboard(gameboard.current_state.bishops);

	std::cout << "Castling Rights (KQkq): " << gameboard.current_state.castleRights << std::endl;
	std::cout << "En Passant Target: " << squareStrings[int(gameboard.current_state.PIPI)] << std::endl;
	std::cout << "To Move: " << ((gameboard.current_state.whiteToMove) ? "White" : "Black") << std::endl;
	std::cout << "Half Move Counter: " << gameboard.current_state.halfmove << std::endl;
	std::cout << "Full Move Counter: " << gameboard.current_state.fullmove << std::endl;

	return 0;
}

