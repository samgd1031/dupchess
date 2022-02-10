
#include <iostream>
#include <bitset>
#include <string>

#include "board.h"


int main()
{
	std::string cmd_string;
	Board gameboard = Board();

	std::cout << "Waiting for input..." << std::endl;
	while (std::getline(std::cin, cmd_string)) {
		// exit if command string is "exit"
		if (_stricmp(cmd_string.c_str(), "exit") == 0) {
			std::cout << "Exiting..." << std::endl;
			break;
		}
		else {
			std::cout << "cmd: " << cmd_string << std::endl;
			std::cout << "Waiting for input..." << std::endl;
		}
	}


	return 0;
}

