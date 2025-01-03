#pragma once

#include <string>
#include <iostream>
#include <time.h>
#include <algorithm>

#include "utils.h"
#include "board.h"
#include "move.h"
#include "../nnue/nnue.h"

// class for the DupChess Engine
// main class to handle UCI commands as well as manage the thinking about chess part
class DupEngine {
public:
	static std::string START_FEN;

	// informational functions
	std::string getEngineName();
	std::string getEngineVersion();
	std::string getAuthorName();
	std::string getEmailAddress();
	std::string getCountryName();

	// game state functions
	bool whiteToMove();

	// move generation
	std::vector<Move> getLegalMoves();
	void chooseMove();
	void makeMove(Move moveToMake);
	void makeMoveFromString(std::string LAN_move);
	void unmakeMove();

	// talk smack to the haters
	std::string trash_talk();

	// print representation of game board/state
	void printGameState();

	//board representation
	Board gameboard;

	// neural network
	NNUE nn;

	// constructor
	DupEngine();

	// utility
	uint64_t perft(int depth);

	Move best_move;

	std::vector<Move> mHistory;
	std::vector<Board> boardHistory;

private:
	std::vector<Move> mlist;

	

	// move generation
	inline void findPawnMoves(std::vector<Move>& mlist, int color, bitboard* color_mask);
	inline void findPawnAttacks(std::vector<Move>& mlist,int sq_index, int color, bitboard* color_mask); // used for castling checks
	inline void findRookMoves(std::vector<Move>& mlist, int sqIndex, int color, bitboard* color_mask);
	inline void findBishopMoves(std::vector<Move>& mlist, int sqIndex, int color, bitboard* color_mask);
	inline void findQueenMoves(std::vector<Move>& mlist, int sqIndex, int color, bitboard* color_mask);
	inline void findKingMoves(std::vector<Move>& mlist, int sqIndex, int color, bitboard* color_mask);
	inline void findKnightMoves(std::vector<Move>& mlist, int sqIndex, int color, bitboard* color_mask);
	inline void findCastles(std::vector<Move>& mlist, int color, bitboard* color_mask, bool can_kingside, bool can_queenside);

	// utility
	int is_attacked(int square_index, int color, bitboard* color_mask); // return the number of pieces attacking a square
	bool is_in_check(int color); // figure out if the king of the given color is in check
	util::Piece getPieceAtIndex(int sq_index); // return the type of piece at the given square
	void reset_game();


};