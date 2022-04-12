
#include <iostream>
#include <bitset>
#include <string>
#include <chrono>


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
			engine.printGameState();

			std::cout << "Waiting for input... (exit to end)" << std::endl;
		}
		// generic test function
		else if(_stricmp(cmd_string.c_str(), "test") == 0) {
			std::cout << "testing a thing" << std::endl;
			int max_moves = 1;
			for (int ii = 0; ii < max_moves; ii++) {
				engine.chooseMove();
				std::cout << "making move: " << engine.chosen_move.getLongSAN() << std::endl;
				engine.makeMove(engine.chosen_move);
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

		// print list of possible moves
		else if (_stricmp(cmd_string.c_str(), "list_moves") == 0) {
			std::vector<Move> mlist = engine.getLegalMoves();

			engine.printGameState();
			std::cout << "Moves for " << ((engine.gameboard.state.whiteToMove) ? "white" : "black") << ":" << std::endl;
			for (int ii = 0; ii < mlist.size(); ii++) {
				std::cout << "\tMove " << ii+1 << ": " << mlist[ii].getLongSAN() << std::endl;
			}

			std::cout << "Waiting for input... (exit to end)" << std::endl;
		}

		// run perft
		else if (cmd_string.compare(0, 6, "perft ") == 0) {
			// get depth
			int depth = std::stoi(cmd_string.substr(6));

			std::cout << "\tPerft for depth " << depth << ": ";

			auto start_time = std::chrono::high_resolution_clock::now();
			uint64_t perft_results = engine.perft(depth);
			auto stop_time = std::chrono::high_resolution_clock::now();
			auto run_time = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time);

			std::cout << perft_results << std::endl;

			std::cout << "\tPerft run time: " << run_time.count() *1e-3 << " ms\n";
			std::cout << "\tSpeed: " << (float)perft_results / (run_time.count() * 1E-6) << " nodes/sec\n";
			std::cout << "Waiting for input... (exit to end)" << std::endl;	
		}

		// run divide (perft but put out the first set of moves)
		else if (cmd_string.compare(0, 7, "divide ") == 0) {
			// get depth
			int depth = std::stoi(cmd_string.substr(7));

			std::cout << "\tPerft for depth " << depth << "\n";

			auto start_time = std::chrono::high_resolution_clock::now();
			std::vector<Move> mlist = engine.getLegalMoves();
			
			uint64_t perft_results, total_nodes = 0;
			for (int ii = 0; ii < mlist.size(); ii++) {
				std::cout << "\t  " << mlist[ii].getLongSAN() << ": ";
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

