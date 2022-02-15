
#include <iostream>
#include <bitset>
#include <string>

#include "dupEngine.h"


int main()
{
	std::string cmd_string;
	DupEngine engine = DupEngine();

	std::cout << "    ____              ________                   " << std::endl;
	std::cout << "   / __ \\__  ______  / ____/ /_  ___  __________ " << std::endl;
	std::cout << "  / / / / / / / __ \\/ /   / __ \\/ _ \\/ ___/ ___/ " << std::endl;
	std::cout << " / /_/ / /_/ / /_/ / /___/ / / /  __(__  |__  )  " << std::endl;
	std::cout << "/_____/\\____/ ____/\\____/_/ /_/\\___/____/____/   " << std::endl;
	std::cout << "           /_/                                     " << std::endl;


	std::cout << "    " << engine.getEngineName() << " v." << engine.getEngineVersion() << std::endl;
	std::cout << "    " << engine.getAuthorName() << std::endl;
	std::cout << "    " << engine.getEmailAddress() << std::endl;
	std::cout << "    " << engine.getCountryName() << std::endl;

	std::cout << "\nWaiting for input... (exit to end)" << std::endl;
	while (std::getline(std::cin, cmd_string)) {
		// exit if command string is "exit"
		if (_stricmp(cmd_string.c_str(), "exit") == 0) {
			std::cout << "Exiting..." << std::endl;
			break;
		}
		//set game board state from a FEN
		else if (cmd_string.compare(0, 8, "set_fen ") == 0) {
			std::string fen = cmd_string.substr(8, cmd_string.length());
			std::cout << "setting board from FEN:\n" << fen << std::endl;

			engine.gameboard.setBoardFromFEN(fen);

			std::cout << "new board state:" << std::endl;
			util::printbitboard((engine.gameboard.current_state.white_pcs | engine.gameboard.current_state.black_pcs));

			std::cout << "Waiting for input... (exit to end)" << std::endl;
		}
		// generic test function
		else if(_stricmp(cmd_string.c_str(), "test") == 0) {
			std::cout << "testing a thing" << std::endl;

			engine.printGameState();
			int max_moves = 5;
			for (int ii = 0; ii < max_moves * 2; ii++) {
				engine.makeMove();
				engine.printGameState();
			}
			

			std::cout << "done testing a thing" << std::endl;
			std::cout << "Waiting for input... (exit to end)" << std::endl;
		}
		//
		else if (_stricmp(cmd_string.c_str(), "print_board") == 0) {
			engine.printGameState();
			std::cout << "Waiting for input... (exit to end)" << std::endl;
		}

		// talk smack
		else if (_stricmp(cmd_string.c_str(), "trash_talk") == 0) {
			std::cout << engine.trash_talk() << std::endl;
			std::cout << "Waiting for input... (exit to end)" << std::endl;
		}
		else {
			std::cout << "cmd: " << cmd_string << std::endl;
			std::cout << "Waiting for input... (exit to end)" << std::endl;
		}
	}


	return 0;
}

