#include "dupEngine.h"


// Constructor ////////////////////////////
DupEngine::DupEngine() {
	gameboard = Board();
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
// /////////////////////
// TODO: double pawn pushes
// TODO: captures
// TODO: en passant
inline void DupEngine::findPawnMoves(std::vector<Move>& mlist) {
	// single pawn push
	bitboard empty = ~(gameboard.current_state.white_pcs | gameboard.current_state.black_pcs);
	bitboard targets;
	int color = (gameboard.current_state.whiteToMove) ? 1 : -1;
	if ((*this).gameboard.current_state.whiteToMove) {
		targets = ((gameboard.current_state.pawns & gameboard.current_state.white_pcs) << 8)  & empty;
	}
	else {
		targets = ((gameboard.current_state.pawns & gameboard.current_state.black_pcs) >> 8) & empty;
	}
	// encode moves
	int movecount = std::_Popcount(targets);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanReverse64(&target_ind, targets);

		mlist.push_back(Move((uint32_t)(target_ind - 8 * color), (uint32_t)target_ind, false, false));
		targets ^= 1ULL << target_ind;
	}


}