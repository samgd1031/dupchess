
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
		else if(_stricmp(cmd_string.c_str(), "test") == 0) {
			std::cout << "testing a thing" << std::endl;

			std::string setFEN = "rnbqkbnr/ppppppp1/7p/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 0 1";
			engine.gameboard.setBoardFromFEN(setFEN);
			std::cout << "current pawn state" << std::endl;
			printbitboard(engine.gameboard.current_state.pawns);
			std::vector<Move> mlist = engine.getLegalMoves();
			std::cout << "moves I found" << std::endl;
			for (int ii = 0; ii < mlist.size(); ii++) {
				std::cout << ii << " - " << mlist[ii].getLongSAN() << std::endl;
			}
			std::cout << "done testing a thing" << std::endl;
			std::cout << "Waiting for input... (exit to end)" << std::endl;
		}
		else {
			std::cout << "cmd: " << cmd_string << std::endl;
			std::cout << "Waiting for input... (exit to end)" << std::endl;
		}
	}


	return 0;
}

