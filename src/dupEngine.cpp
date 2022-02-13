#include "dupEngine.h"


// Constructor ////////////////////////////
DupEngine::DupEngine() {
	gameboard = Board();
	mlist.reserve(200);
	gameboard.setBoardFromFEN(DupEngine::START_FEN);
}
///////////////////////////////////////////

// informational functions ////////////////
std::string DupEngine::getEngineName() {
	return "DupChess";
}

std::string DupEngine::getEngineVersion() {
	return "0.0.1";
}

std::string DupEngine::getAuthorName() {
	return "Sam Dupas";
}

std::string DupEngine::getEmailAddress() {
	return "samgd1031@gmail.com";
}

std::string DupEngine::getCountryName() {
	return "USA";
}
///////////////////////////////////////////

// Trash talk /////////////////////////////
std::string DupEngine::trash_talk() {
	std::string lines[3] = { "Up yours, Jonah!\n",
							 "Bite my shiny metal ass!\n",
							 "I'm afraid I can't let you do that...\n" };

	return lines[std::rand() % 3];
}
///////////////////////////////////////////

// game start string
std::string DupEngine::START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Game Status Functions //////////////////
bool DupEngine::whiteToMove() {
	return (*this).gameboard.current_state.whiteToMove;
}
///////////////////////////////////////////


// Move Generation
std::vector<Move> DupEngine::getLegalMoves() {
	std::vector<Move> movelist;
	DupEngine::findPawnMoves(movelist);

	return movelist;
}

// return a move list of all legal pawn moves
// currently does:
// single pawn pushes
// double pawn pushes
// /////////////////////
// TODO: captures
// TODO: en passant
inline void DupEngine::findPawnMoves(std::vector<Move>& mlist) {
	bitboard empty = ~(gameboard.current_state.white_pcs | gameboard.current_state.black_pcs);
	bitboard single_tgts, dbl_tgts;
	int color = (gameboard.current_state.whiteToMove) ? 1 : -1;
	// get a pointer mask to only return pawns of the current color to move
	bitboard *color_mask = (color == 1) ? &gameboard.current_state.white_pcs : &gameboard.current_state.black_pcs;
	// single pawn push targets are the empty squares on the next file
	single_tgts = util::genShift((gameboard.current_state.pawns & *color_mask), color * 8) & empty;

	// double pawn push targets are the square on the next file after the single target as long as pawn is on file 2 (white) or 7 (black)
	// single targets will be masked out on file 3 (this means the pawn is on file 2) or file 6 for black (black pawns on file 7)
	bitboard dbl_mask = (color == 1) ? (bitboard)0x0000000000FF0000 : (bitboard)0x0000FF0000000000;
	dbl_tgts = util::genShift((single_tgts & dbl_mask), color * 8) & empty;

	// encode single moves and add to move list
	int movecount = std::_Popcount(single_tgts);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanReverse64(&target_ind, single_tgts);
		mlist.push_back(Move((uint32_t)(target_ind - 8 * color), (uint32_t)target_ind, false, false));
		single_tgts ^= 1ULL << target_ind;
	}

	// encode double moves and add to move list
	movecount = std::_Popcount(dbl_tgts);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanReverse64(&target_ind, dbl_tgts);
		mlist.push_back(Move((uint32_t)(target_ind - 16 * color), (uint32_t)target_ind, false, false));
		dbl_tgts ^= 1ULL << target_ind;
	}


}