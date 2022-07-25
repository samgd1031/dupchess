
#include <iostream>
#include <bitset>
#include <string>
#include <chrono>


#include "chess/io_handler.h"
#include "nnue/nnue.h"


int main()
{
	std::string model_file = "D:/dupchess/src/nnue/RUN1_100EP_ep15.bindat";
	NNUE nn(model_file);
	//IO_handler io = IO_handler();
	//io.start();


	return 0;
}

