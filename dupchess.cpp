#include <iostream>
#include <bitset>
#include "include/global.h"

using namespace std;

int main()
{
	BOARD_S test_board;
	test_board.white_pcs = 0x000000000000FFFF;

	printf("White Pieces");
	printbitboard(test_board.white_pcs);

	test_board.black_pcs = 0xFFFF000000000000;

	printf("\n\n\nBlack Pieces");
	printbitboard(test_board.black_pcs);


	printf("\n\n\nAll Pieces");
	printbitboard(test_board.white_pcs | test_board.black_pcs);
	return 0;
}

