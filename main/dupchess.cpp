
#include <iostream>
#include <bitset>
#include <string>
#include <chrono>

#include "chess/io_handler.h"
#include "nnue/nnue.h"
#include "chess/board.h"

using namespace std;
int main()
{
	DupEngine eng;
	NNUE nn;
	Board b;
	eng.gameboard.setBoardFromFEN("8/8/5Q2/2k5/4K3/8/8/8 w - - 0 1");
	nn.get_active_features(eng.gameboard);
	nn.refresh_accumulator();
	eng.printGameState();
	float score = nn.eval();
	cout << "Score: " << score << endl;

	//IO_handler io = IO_handler();
	//io.start();


	return 0;
}

