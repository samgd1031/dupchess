#include "io_handler.h"

/// <summary>
/// constructor
/// </summary>
IO_handler::IO_handler(){
	engine = DupEngine();
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
}

/// <summary>
/// loop that listens for incoming commmands and sends appropriate outputs through stdin/out
/// </summary>
void IO_handler::process_io() {
	std::string cmd_string;
	while (std::getline(std::cin, cmd_string)) {
		// exit if command string is "exit"
		if (cmd_string.compare("exit") == 0) {
			std::cout << "Exiting...";
			break;
		}
		//set game board state from a FEN
		else if (cmd_string.compare(0, 4, "fen ") == 0) {
			std::string fen = cmd_string.substr(4, cmd_string.length());
			std::cout << "setting board from FEN:\n" << fen << std::endl;

			engine.gameboard.setBoardFromFEN(fen);

			std::cout << "new board state:" << std::endl;
			engine.printGameState();
		}
		// generic test function
		else if (cmd_string.compare("test") == 0) {
				engine.chooseMove();
				std::cout << "making move: " << engine.chosen_move.getLongAN() << std::endl;
				engine.makeMove(engine.chosen_move);
				engine.printGameState();
		}
		//
		else if (cmd_string.compare("pr") == 0) {
			engine.printGameState();
		}

		// print list of possible moves
		else if (cmd_string.compare("lm") == 0) {
			std::vector<Move> mlist = engine.getLegalMoves();

			engine.printGameState();
			std::cout << "Moves for " << ((engine.gameboard.state.whiteToMove) ? "white" : "black") << ":" << std::endl;
			for (int ii = 0; ii < mlist.size(); ii++) {
				std::cout << "Move " << std::setw(2) << ii + 1 << ": " << mlist[ii].getLongAN() << std::endl;
			}
		}

		// run divide (perft but put out the first set of moves)
		else if (cmd_string.compare(0, 6, "perft ") == 0) {
			// get depth
			int depth = std::stoi(cmd_string.substr(6));

			auto start_time = std::chrono::high_resolution_clock::now();
			std::vector<Move> mlist = engine.getLegalMoves();

			uint64_t perft_results, total_nodes = 0;
			for (int ii = 0; ii < mlist.size(); ii++) {
				std::cout << mlist[ii].getLongAN() << ": ";
				engine.makeMove(mlist[ii]);
				perft_results = engine.perft(depth - 1);
				engine.unmakeMove();

				std::cout << perft_results << "\n";
				total_nodes += perft_results;
			}

			auto stop_time = std::chrono::high_resolution_clock::now();
			auto run_time = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time);

			std::cout << "\n\tTotal Nodes: " << total_nodes << "\n";
			std::cout << "\tPerft run time: " << run_time.count() * 1e-3 << " ms\n";
			std::cout << "\tSpeed: " << (float)total_nodes / (run_time.count() * 1E-6) << " nodes/sec\n";
		}
		// uci start command
		else if(cmd_string.compare("uci") == 0) {
			respond_uci();
		}
		// talk smack
		else if (cmd_string.compare("smack") == 0) {
			std::cout << engine.trash_talk() << std::endl;
		}
		// list commands
		else if (cmd_string.compare("help") == 0) {
			std::cout << "--- LIST OF COMMANDS ---\n";
			std::cout << "\tfen <FEN_STRING> - Set board to position indicated by FEN_STRING\n";
			std::cout << "\tuci - uci start command\n";
			std::cout << "\ttest - make a random move\n";
			std::cout << "\tpr - print the current board state\n";
			std::cout << "\tlm - list all available moves for the current player\n";
			std::cout << "\tperft <DEPTH> - run a perft test to depth DEPTH from the current position\n";
			std::cout << "\tsmack - talk smack\n";
			std::cout << "\texit - close the program\n";
		}
		else {
			std::cout << "cmd: " << cmd_string << " not recognized" << std::endl;

		}
	}
}

/// <summary>
/// start processing io
/// </summary>
void IO_handler::start() {
	std::thread io_thread(&IO_handler::process_io, this);
	io_thread.join();
}


/// <summary>
/// respond to "uci" command from gui
/// </summary>
void IO_handler::respond_uci(){
	std::cout << "id name DupChess" << std::endl;;
	std::cout << "id author Sam Dupas" << std::endl;;

	//TODO: add options if I ever get smart enough to figure that out

	std::cout << "uciok" << std::endl;
}