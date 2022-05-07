
#include <iostream>
#include <bitset>
#include <string>
#include <chrono>


#include "dupEngine.h"


int main()
{

	DupEngine engine = DupEngine();

	/*
	while (std::getline(std::cin, cmd_string)) {
		// exit if command string is "exit"
		if (_stricmp(cmd_string.c_str(), "exit") == 0) {
			std::cout << "Exiting..." << std::endl;
			break;
		}
		//set game board state from a FEN
		else if (cmd_string.compare(0, 4, "fen ") == 0) {
			std::string fen = cmd_string.substr(8, cmd_string.length());
			std::cout << "setting board from FEN:\n" << fen << std::endl;

			engine.gameboard.setBoardFromFEN(fen);

			std::cout << "new board state:" << std::endl;
			engine.printGameState();
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
		}
		//
		else if (cmd_string.compare(0,2, "pr") == 0) {
			engine.printGameState();
		}

		// print list of possible moves
		else if (cmd_string.compare(0,2, "lm") == 0) {
			std::vector<Move> mlist = engine.getLegalMoves();

			engine.printGameState();
			std::cout << "Moves for " << ((engine.gameboard.state.whiteToMove) ? "white" : "black") << ":" << std::endl;
			for (int ii = 0; ii < mlist.size(); ii++) {
				std::cout << "\tMove " << ii+1 << ": " << mlist[ii].getLongSAN() << std::endl;
			}
		}

		// run divide (perft but put out the first set of moves)
		else if (cmd_string.compare(0, 6, "perft ") == 0) {
			// get depth
			int depth = std::stoi(cmd_string.substr(6));

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
		}

		// talk smack
		else if (cmd_string.compare(0, 5, "smack") == 0) {
			std::cout << engine.trash_talk() << std::endl;
		}
		// list commands
		else if(cmd_string.compare(0, 4, "help") == 0) {
			std::cout << "--- LIST OF COMMANDS ---\n";
			std::cout << "\tfen <FEN_STRING> - Set board to position indicated by FEN_STRING\n";
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
	*/

	return 0;
}

