
#include <iostream>
#include <bitset>
#include <string>
#include <chrono>

#include "chess/io_handler.h"
#include "nnue/nnue.h"
#include "chess/board.h"

using namespace std;
int main(int argc, char* argv[])
{
	DupEngine eng;
	cout << argv[0] << endl;
	NNUE nn;
	Board b;
	eng.gameboard.setBoardFromFEN("8/2K5/8/1P3k2/8/8/8/7r b - - 0 1");
	nn.get_active_features(eng.gameboard);
	nn.refresh_accumulator();
	eng.printGameState();
	float score = nn.eval();
	cout << "Score: " << score << endl;

	//IO_handler io = IO_handler();
	//io.start();


	return 0;
}

