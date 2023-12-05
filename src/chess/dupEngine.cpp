#include "dupEngine.h"


// Constructor ////////////////////////////
DupEngine::DupEngine(void) {

	gameboard = Board();
	mlist.reserve(200);
	mHistory.reserve(80);
	boardHistory.reserve(40);
	gameboard.setBoardFromFEN(DupEngine::START_FEN);
	best_move = Move();
	srand((int)time(NULL));  // set random generator seed
	nn.refresh_accumulator();
}
///////////////////////////////////////////

/// <summary>
/// reset game state to initial board
/// </summary>
void DupEngine::reset_game() {
	gameboard = Board();
	mlist.clear();
	mHistory.clear();
	boardHistory.clear();
	gameboard.setBoardFromFEN(DupEngine::START_FEN);
	best_move = Move();
}

// informational functions ////////////////
std::string DupEngine::getEngineName() {
	return "DupChess";
}

std::string DupEngine::getEngineVersion() {
	return "1.0.2";
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
	std::string lines[5] = { "Up yours!",
							 "Bite my shiny metal ass!",
							 "I'm afraid I can't let you do that...",
							 "'Not only do I smell bad, I'm horrible at chess!' - You",
							 "I'm playing DupChess while you're playing poopChess"};

	return lines[std::rand() % 5];
}
///////////////////////////////////////////

// game start string
std::string DupEngine::START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Game Status Functions //////////////////
bool DupEngine::whiteToMove() {
	return (*this).gameboard.state.whiteToMove;
}
///////////////////////////////////////////


// Move Generation
std::vector<Move> DupEngine::getLegalMoves() {
	int color = (gameboard.state.whiteToMove) ? 1 : -1;
	// get a pointer mask to only return pieces of the current color to move
	bitboard* color_mask = (color == 1) ? &gameboard.state.white_pcs : &gameboard.state.black_pcs;

	std::vector<Move> movelist;  // will hold all the possible moves

	// pawns
	DupEngine::findPawnMoves(movelist, color, color_mask);

	// rooks
	bitboard rooks_to_move = gameboard.state.rooks & *color_mask;
	int n_rooks = popcount64(rooks_to_move);
	for (int ii = 0; ii < n_rooks; ii++) {
		unsigned long rook_index;
		bitscanfwd_u64(&rook_index, rooks_to_move);
		DupEngine::findRookMoves(movelist, rook_index, color, color_mask);
		rooks_to_move &= ~(1ULL << rook_index);
	}

	// bishops
	bitboard bishop_to_move = gameboard.state.bishops & *color_mask;
	int n_bishop = popcount64(bishop_to_move);
	for (int ii = 0; ii < n_bishop; ii++) {
		unsigned long bishop_index;
		bitscanfwd_u64(&bishop_index, bishop_to_move);
		DupEngine::findBishopMoves(movelist, bishop_index, color, color_mask);
		bishop_to_move &= ~(1ULL << bishop_index);
	}

	//knights
	bitboard knight_to_move = gameboard.state.knights & *color_mask;
	int n_knight = popcount64(knight_to_move);
	for (int ii = 0; ii < n_knight; ii++) {
		unsigned long knight_index;
		bitscanfwd_u64(&knight_index, knight_to_move);
		DupEngine::findKnightMoves(movelist, knight_index, color, color_mask);
		knight_to_move &= ~(1ULL << knight_index);
	}
	
	// queens
	bitboard queens_to_move = gameboard.state.queens & *color_mask;
	int n_queen = popcount64(queens_to_move);
	for (int ii = 0; ii < n_queen; ii++) {
		unsigned long queen_index;
		bitscanfwd_u64(&queen_index, queens_to_move);
		DupEngine::findQueenMoves(movelist, queen_index, color, color_mask);
		queens_to_move &= ~(1ULL << queen_index);
	}

	// king
	bitboard king_to_move = gameboard.state.kings & *color_mask;
	unsigned long king_index = 0;
	bitscanfwd_u64(&king_index, king_to_move);
	DupEngine::findKingMoves(movelist, king_index, color, color_mask);

	//Castling
	bool can_kingside = (color == 1) ? (bool)gameboard.state.castleRights.test(3) : (bool)gameboard.state.castleRights.test(1);
	bool can_queenside = (color == 1) ? (bool)gameboard.state.castleRights.test(2) : (bool)gameboard.state.castleRights.test(0);
	DupEngine::findCastles(movelist, color, color_mask, can_kingside, can_queenside);

	//remove moves that would leave the king in check
	for (int ii = 0; ii < movelist.size(); ii++) {
		// make the move
		makeMove(movelist[ii]);
		
		// see if king is attacked (would put the king in check)
		unsigned long test_king_index;
		bitscanfwd_u64(&test_king_index, gameboard.state.kings & *color_mask);
		bool is_king_attacked = is_attacked(test_king_index, color, color_mask);
		// if king is attacked, remove this move from valid move list
		if (is_king_attacked) { movelist.erase(movelist.begin() + ii); ii--; }

		// unmake the move
		unmakeMove();

	}
	

	return movelist;
}

// return a move list of all possible pawn moves
// currently does:
// single pawn pushes
// double pawn pushes
// captures
// promotions
// en passant
// /////////////////////
inline void DupEngine::findPawnMoves(std::vector<Move>& mlist, int color ,bitboard* color_mask) {
	bitboard empty = ~(gameboard.state.white_pcs | gameboard.state.black_pcs);
	bitboard single_tgts, dbl_tgts;
	// single pawn push targets are the empty squares on the next file
	single_tgts = util::genShift((gameboard.state.pawns & *color_mask), color * 8) & empty;

	// double pawn push targets are the square on the next file after the single target as long as pawn is on file 2 (white) or 7 (black)
	// single targets will be masked out on file 3 (this means the pawn is on file 2) or file 6 for black (black pawns on file 7)
	bitboard dbl_mask = (color == 1) ? (bitboard)0x0000000000FF0000 : (bitboard)0x0000FF0000000000;
	dbl_tgts = util::genShift((single_tgts & dbl_mask), color * 8) & empty;

	// pawn attack targets are the squares to the northeast (southwest) and northwest (southeast) for white/black
	// to avoid A/H file pawns the appropriate rank will be masked out
	// space must also be occupied by a piece of the opposite color to be a valid attack target
	bitboard east_mask = (color == 1) ? 0x7F7F7F7F7F7F7F7F : 0xFEFEFEFEFEFEFEFE;
	bitboard west_mask = (color == 1) ? 0xFEFEFEFEFEFEFEFE : 0x7F7F7F7F7F7F7F7F;

	bitboard east_atk = util::genShift((gameboard.state.pawns & *color_mask) & east_mask, color * 9) & (~empty & ~*color_mask);
	bitboard west_atk = util::genShift((gameboard.state.pawns & *color_mask) & west_mask, color * 7) & (~empty & ~*color_mask);


	// encode single moves and add to move list
	int movecount = popcount64(single_tgts);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, single_tgts);
		// if pawn makes it to the end add all possible promotion moves
		if(((color == 1) && (target_ind / 8 == 7)) || ((color == -1) && (target_ind / 8 == 0))){
			for (uint8_t jj = 1; jj < 5; jj++) {
				mlist.push_back(Move((uint32_t)(target_ind - 8 * color), (uint32_t)target_ind, util::Piece::PAWN, false, true, false, false, jj));
			}
		}
		else{ // no promotion
			mlist.push_back(Move((uint32_t)(target_ind - 8 * color), (uint32_t)target_ind, util::Piece::PAWN, false, false, false, false, 0));
		}
		single_tgts ^= 1ULL << target_ind;
	}

	// encode double moves and add to move list
	movecount = popcount64(dbl_tgts);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, dbl_tgts);
		mlist.push_back(Move((uint32_t)(target_ind - 16 * color), (uint32_t)target_ind, util::Piece::PAWN, false, false, true, false, 0));
		dbl_tgts ^= 1ULL << target_ind;
	}

	// encode captures and add to move list
	movecount = popcount64(east_atk);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, east_atk);
		// if pawn makes it to the end add all possible promotion moves
		if (((color == 1) && (target_ind / 8 == 7)) || ((color == -1) && (target_ind / 8 == 0))) {
			for (uint8_t jj = 1; jj < 5; jj++) {
				mlist.push_back(Move((uint32_t)(target_ind - 9 * color), (uint32_t)target_ind, util::Piece::PAWN, true, true, false, false, jj));
			}
		}
		else{
			mlist.push_back(Move((uint32_t)(target_ind - 9 * color), (uint32_t)target_ind, util::Piece::PAWN, true, false, false, false, 0));
		}
		east_atk ^= 1ULL << target_ind;
	}
	
	movecount = popcount64(west_atk);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, west_atk);
		// if pawn makes it to the end add all possible promotion moves
		if (((color == 1) && (target_ind / 8 == 7)) || ((color == -1) && (target_ind / 8 == 0))) {
			for (uint8_t jj = 1; jj < 5; jj++) {
				mlist.push_back(Move((uint32_t)(target_ind - 7 * color), (uint32_t)target_ind, util::Piece::PAWN, true, true, false, false, jj));
			}
		}
		else{
			mlist.push_back(Move((uint32_t)(target_ind - 7 * color), (uint32_t)target_ind, util::Piece::PAWN, true, false, false, false, 0));
		}
		west_atk ^= 1ULL << target_ind;
	}

	// check en passant
	if (gameboard.state.PIPI != util::squares::EMPTY_SQ) {
		bitboard epTarget = 1ULL << (int)gameboard.state.PIPI;

		bitboard east_atk = util::genShift((gameboard.state.pawns & *color_mask) & east_mask, color * 9) & epTarget;
		bitboard west_atk = util::genShift((gameboard.state.pawns & *color_mask) & west_mask, color * 7) & epTarget;

		// encode moves
		movecount = popcount64(east_atk);
		for (int ii = 0; ii < movecount; ii++) {
			unsigned long target_ind;
			bitscanfwd_u64(&target_ind, east_atk);
			mlist.push_back(Move((uint32_t)(target_ind - 9 * color), (uint32_t)target_ind, util::Piece::PAWN, true, false, true, false, 0));
			east_atk ^= 1ULL << target_ind;
		}

		movecount = popcount64(west_atk);
		for (int ii = 0; ii < movecount; ii++) {
			unsigned long target_ind;
			bitscanfwd_u64(&target_ind, west_atk);
			mlist.push_back(Move((uint32_t)(target_ind - 7 * color), (uint32_t)target_ind, util::Piece::PAWN, true, false, true, false, 0));
			east_atk ^= 1ULL << target_ind;
		}
	}
}


/// <summary>
/// find the potential pawn attacks assuming there is a pawn of the specified color at sq_index
/// This function is used during castling checks to make sure no pawns are attacking any of the castling squares
/// </summary>
/// <param name="mlist"></param>
/// <param name="sq_index"></param>
/// <param name="color"></param>
/// <param name="color_mask"></param>
inline void DupEngine::findPawnAttacks(std::vector<Move>& mlist, int sq_index, int color, bitboard* color_mask) {
	bitboard empty = ~(gameboard.state.white_pcs | gameboard.state.black_pcs);
	// pawn attack targets are the squares to the northeast (southwest) and northwest (southeast) for white/black
	// to avoid A/H file pawns the appropriate rank will be masked out
	// space must also be occupied by a piece of the opposite color to be a valid attack target
	bitboard east_mask = (color == 1) ? 0x7F7F7F7F7F7F7F7F : 0xFEFEFEFEFEFEFEFE;
	bitboard west_mask = (color == 1) ? 0xFEFEFEFEFEFEFEFE : 0x7F7F7F7F7F7F7F7F;

	bitboard east_atk = util::genShift((1ULL << sq_index) & east_mask, color * 9) & (~empty & ~*color_mask);
	bitboard west_atk = util::genShift((1ULL << sq_index) & west_mask, color * 7) & (~empty & ~*color_mask);

	// encode captures and add to move list
	int movecount = popcount64(east_atk);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, east_atk);
		// if pawn makes it to the end add all possible promotion moves
		if (((color == 1) && (target_ind / 8 == 7)) || ((color == -1) && (target_ind / 8 == 0))) {
			for (uint8_t jj = 1; jj < 5; jj++) {
				mlist.push_back(Move((uint32_t)(target_ind - 9 * color), (uint32_t)target_ind, util::Piece::PAWN, true, true, false, false, jj));
			}
		}
		else {
			mlist.push_back(Move((uint32_t)(target_ind - 9 * color), (uint32_t)target_ind, util::Piece::PAWN, true, false, false, false, 0));
		}
		east_atk ^= 1ULL << target_ind;
	}

	movecount = popcount64(west_atk);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, west_atk);
		// if pawn makes it to the end add all possible promotion moves
		if (((color == 1) && (target_ind / 8 == 7)) || ((color == -1) && (target_ind / 8 == 0))) {
			for (uint8_t jj = 1; jj < 5; jj++) {
				mlist.push_back(Move((uint32_t)(target_ind - 7 * color), (uint32_t)target_ind, util::Piece::PAWN, true, true, false, false, jj));
			}
		}
		else {
			mlist.push_back(Move((uint32_t)(target_ind - 7 * color), (uint32_t)target_ind, util::Piece::PAWN, true, false, false, false, 0));
		}
		west_atk ^= 1ULL << target_ind;
	}
}

/// <summary>
/// given a pointer to a move list and index, append to the list all possible rook moves
/// rook moves are along ranks or files
/// </summary>
/// <param name="mlist"></param>
/// <param name="sqIndex"></param>
inline void DupEngine::findRookMoves(std::vector<Move>& mlist, int sqIndex, int color, bitboard* color_mask) {
	int rank = sqIndex / 8;
	int file = sqIndex % 8;

	bitboard occ = (gameboard.state.white_pcs | gameboard.state.black_pcs);

	// vertical moves with hyperbolic quintessence
	bitboard vert = util::hyp_quint(occ, util::fileMasks[file], sqIndex);

	// horizontal moves cant use hyperbolic quintessence because of dumb math reason
	// this is probably a slowish way to do it (lookup tables maybe faster but I'm dumb)
	bitboard horz = util::rankAttacks(occ, sqIndex);


	// get quiescent moves
	bitboard rook_moves = (vert | horz) & ~occ;

	// AND with inverse of color mask to remove the possibility of capturing own pieces
	bitboard rook_atks = ((vert | horz) & occ) & ~*color_mask;

	// encode moves
	int n_moves = popcount64(rook_moves);
	for (int ii = 0; ii < n_moves; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, rook_moves);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::ROOK, false, false, false, false, 0));
		rook_moves ^= 1ULL << target_ind;
	}

	// encode captures
	n_moves = popcount64(rook_atks);
	for (int ii = 0; ii < n_moves; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, rook_atks);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::ROOK, true, false, false, false, 0));
		rook_atks ^= 1ULL << target_ind;
	}
}

/// <summary>
/// given pointer to move list, square index, and color info, append to mlist all possible bishop moves
/// </summary>
/// <param name="mlist"></param>
/// <param name="sqIndex"></param>
/// <param name="color"></param>
/// <param name="color_mask"></param>
inline void DupEngine::findBishopMoves(std::vector<Move>& mlist, int sqIndex, int color, bitboard* color_mask) {
	int rank = sqIndex / 8;
	int file = sqIndex % 8;

	bitboard occ = (gameboard.state.white_pcs | gameboard.state.black_pcs);

	// moves along diagonal
	bitboard diag = util::hyp_quint(occ, util::diagMasks[(rank - file) & 15], sqIndex);

	// moves along anti diagonal
	bitboard anti_diag = util::hyp_quint(occ, util::adiagMasks[(rank + file) ^ 7], sqIndex);

	// get quiescent moves
	bitboard bishop_moves = (diag | anti_diag) & ~occ;

	// get attacks (collisions with pieces of opposite color)
	bitboard bishop_atks = (diag | anti_diag) & (occ & ~*color_mask);

	// encode moves
	int n_moves = popcount64(bishop_moves);
	for (int ii = 0; ii < n_moves; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, bishop_moves);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::BISHOP, false, false, false, false, 0));
		bishop_moves ^= 1ULL << target_ind;
	}

	// encode captures
	n_moves = popcount64(bishop_atks);
	for (int ii = 0; ii < n_moves; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, bishop_atks);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::BISHOP, true, false, false, false, 0));
		bishop_atks ^= 1ULL << target_ind;
	}
}

/// <summary>
/// Find all possible queen moves (union of bishop and rook moves from a square)
/// </summary>
/// <param name="mlist"></param>
/// <param name="sqIndex"></param>
/// <param name="color"></param>
/// <param name="color_mask"></param>
inline void DupEngine::findQueenMoves(std::vector<Move>& mlist, int sqIndex, int color, bitboard* color_mask) {
	std::vector<Move> temp_mlist;

	// find rook and bishop moves
	findRookMoves(temp_mlist, sqIndex, color, color_mask);
	findBishopMoves(temp_mlist, sqIndex, color, color_mask);

	// set piece ID to queen
	for (int ii = 0; ii < temp_mlist.size(); ii++) {
		temp_mlist[ii].setPieceID(util::Piece::QUEEN);
	}

	// append these moves to the move list
	std::move(temp_mlist.begin(), temp_mlist.end(), std::back_inserter(mlist));
}

/// <summary>
/// Find all possible king moves
/// </summary>
/// <param name="mlist"></param>
/// <param name="sqIndex"></param>
/// <param name="color"></param>
/// <param name="color_mask"></param>
inline void DupEngine::findKingMoves(std::vector<Move>& mlist, int sqIndex, int color, bitboard* color_mask){
	// get bitboard of friendly & enemy pieces
	bitboard friends = (gameboard.state.white_pcs | gameboard.state.black_pcs) & *color_mask;
	bitboard enemies = (gameboard.state.white_pcs | gameboard.state.black_pcs) ^ friends;

	// add squares around enemy king to "friends" so that it will not try to enter it
	unsigned long enemy_king_ind = 0;
	bitscanfwd_u64(&enemy_king_ind, (gameboard.state.kings & enemies));
	bitboard enemy_king = util::genShift(util::kingAttack, (enemy_king_ind - util::kingAttackIndex));
	if (enemy_king_ind % 8 == 0) { // A file
		enemy_king &= ~(util::fileMasks[7]); // mask off H file
	}
	else if (enemy_king_ind % 8 == 7) { // H file
		enemy_king &= ~(util::fileMasks[0]); // mask off A file
	}
	friends |= enemy_king;

	// get bitboard of all neighboring squares, masking off A/H files as necessary
	// bitwise shift does not allow for overflow on ranks 1/8 so no masking is necessary for ranks
	bitboard king_targets = util::genShift(util::kingAttack, (sqIndex - util::kingAttackIndex));
	if (sqIndex % 8 == 0) { // A file
		king_targets &= ~(util::fileMasks[7]); // mask off H file
	}
	else if (sqIndex % 8 == 7) { // H file
		king_targets &= ~(util::fileMasks[0]); // mask off A file
	}
	// mask out friends
	king_targets &= ~(friends);

	// split into move and capture bitboards
	bitboard king_captures = king_targets & enemies;
	bitboard king_moves = king_targets & ~(king_captures);

	// encode moves
	int n_moves = popcount64(king_moves);
	for (int ii = 0; ii < n_moves; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, king_moves);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::KING, false, false, false, false, 0));
		king_moves ^= 1ULL << target_ind;
	}

	// encode captures
	int n_caps = popcount64(king_captures);
	for (int ii = 0; ii < n_caps; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, king_captures);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::KING, true, false, false, false, 0));
		king_captures ^= 1ULL << target_ind;
	}
}

/// <summary>
/// Find all possible knight moves
/// </summary>
/// <param name="mlist"></param>
/// <param name="sqIndex"></param>
/// <param name="color"></param>
/// <param name="color_mask"></param>
inline void DupEngine::findKnightMoves(std::vector<Move>& mlist, int sqIndex, int color, bitboard* color_mask) {
	// get bitboard of friendly & enemy pieces
	bitboard friends = (gameboard.state.white_pcs | gameboard.state.black_pcs) & *color_mask;
	bitboard enemies = (gameboard.state.white_pcs | gameboard.state.black_pcs) ^ friends;

	// get bitboard of all neighboring squares, masking off AB/GH files as necessary
	// bitwise shift does not allow for overflow on ranks 1/8 so no masking is necessary for ranks
	bitboard knight_targets = util::genShift(util::knight_attack, (sqIndex - util::knight_attack_index));
	if (sqIndex % 8 == 0 || sqIndex % 8 == 1) { // A or B file
		knight_targets &= ~(util::fileMasks[6] | util::fileMasks[7]); // mask off G & H file
	}
	else if (sqIndex % 8 == 7 || sqIndex % 8 == 6) { // G or H file
		knight_targets &= ~(util::fileMasks[0] | util::fileMasks[1]); // mask off A & B file
	}
	// mask out friends
	knight_targets &= ~(friends);

	// split into move and capture bitboards
	bitboard knight_captures = knight_targets & enemies;
	bitboard knight_moves = knight_targets & ~(knight_captures);

	// encode moves
	int n_moves = popcount64(knight_moves);
	for (int ii = 0; ii < n_moves; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, knight_moves);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::KNIGHT, false, false, false, false, 0));
		knight_moves ^= 1ULL << target_ind;
	}

	// encode captures
	int n_caps = popcount64(knight_captures);
	for (int ii = 0; ii < n_caps; ii++) {
		unsigned long target_ind;
		bitscanfwd_u64(&target_ind, knight_captures);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::KNIGHT, true, false, false, false, 0));
		knight_captures ^= 1ULL << target_ind;
	}
}

/// <summary>
/// 
/// </summary>
/// <param name="mlist"></param>
/// <param name="color"></param>
/// <param name="color_mask"></param>
/// <param name="can_kingside"></param>
/// <param name="can_queenside"></param>
inline void DupEngine::findCastles(std::vector<Move>& mlist, int color, bitboard* color_mask, bool can_kingside, bool can_queenside){
	bitboard empty = ~(gameboard.state.white_pcs | gameboard.state.black_pcs);
	util::squares king_location = (color == 1) ? (util::squares::E1) : (util::squares::E8);
	util::squares kingside_tgt = (color == 1) ? (util::squares::G1) : (util::squares::G8);
	util::squares queenside_tgt = (color == 1) ? (util::squares::C1) : (util::squares::C8);
	
	// kingside castle
	if (can_kingside) {
		bitboard kingside_mask = (color == 1) ? (3 * 1ULL << 5) : (3 * 1ULL << 61);
		// first, check squares between king and rook are empty
		if((empty & kingside_mask) == kingside_mask){
			// check king and squares in between for attacks
			bool safe_to_castle = true;
			util::squares squares_to_check[3];
			if (color == 1) {
				squares_to_check[0] = util::squares::E1;
				squares_to_check[1] = util::squares::F1;
				squares_to_check[2] = util::squares::G1;
			}
			else {
				squares_to_check[0] = util::squares::E8;
				squares_to_check[1] = util::squares::F8;
				squares_to_check[2] = util::squares::G8;
			}
			for (int ii = 0; ii < 3; ii++) {
				int num_attackers = is_attacked((int)squares_to_check[ii], color, color_mask);
				if (num_attackers > 0) {
					safe_to_castle = false;
					break;
				}
			}
			
			if (safe_to_castle) {
				mlist.push_back(Move((int)king_location, (int)kingside_tgt, util::Piece::KING, false, false, false, true, 1));
			}	
		}
	}
	
	if (can_queenside) {
		bitboard queenside_mask = (color == 1) ? (7 * 1ULL << 1) : (7 * 1ULL << 57);
		// first, check squares between king and rook are empty
		if ((empty & queenside_mask) == queenside_mask) {
			// check king and squares in between for attacks
			bool safe_to_castle = true;
			util::squares squares_to_check[3];
			if (color == 1) {
				squares_to_check[0] = util::squares::E1;
				squares_to_check[1] = util::squares::D1;
				squares_to_check[2] = util::squares::C1;
			}
			else {
				squares_to_check[0] = util::squares::E8;
				squares_to_check[1] = util::squares::D8;
				squares_to_check[2] = util::squares::C8;
			}
			for (int ii = 0; ii < 3; ii++) {
				int num_attackers = is_attacked((int)squares_to_check[ii], color, color_mask);
				if (num_attackers > 0) {
					safe_to_castle = false;
					break;
				}
			}

			if (safe_to_castle) {
				mlist.push_back(Move((int)king_location, (int)queenside_tgt, util::Piece::KING, false, false, false, true, 2));
			}
		}
	}
}

/// <summary>
/// 
/// </summary>
void DupEngine::chooseMove() {
	// get list of all legal moves
	mlist = getLegalMoves();
	
	// score each move with NNUE (remember that network output is centipawn eval from white's perspective)
	for (int ii = 0; ii<mlist.size(); ++ii)
	{
		makeMove(mlist[ii]);
		nn.get_active_features(gameboard);
		nn.refresh_accumulator();
		mlist[ii].setScore(nn.eval());
		unmakeMove();
	}

	if (mlist.size() == 0) {
		printf("%s has no moves!\n", (gameboard.state.whiteToMove) ? "White" : "Black");
		return;
	}
	
	// pick move that leads to the best eval (most positive/max for white, most negative/min for black)
	// NNUE output always from white's perspective
	int best_move_ind;
	if (whiteToMove())	{ best_move_ind = std::max_element(mlist.begin(), mlist.end()) - mlist.begin(); }
	else{ best_move_ind = std::min_element(mlist.begin(), mlist.end()) - mlist.begin(); }

	best_move = mlist[best_move_ind];
}


/// <makeMove>
/// Take the current list of moves and make one
/// for now will be a random move, eventually some smart eval stuff will need to happen
/// </makeMove>
void DupEngine::makeMove(Move moveToMake) {

	// process the move and update game state accordingly
	boardHistory.push_back(gameboard); // save state to history before making changes
	util::Piece pieceID = moveToMake.getPieceID();
	int color = (gameboard.state.whiteToMove) ? 1 : -1;
	// get piece that's moving, origin and destination squares
	int fromSq = moveToMake.getFromSquare();
	int toSq = moveToMake.getToSquare();
	mHistory.push_back(moveToMake);

	// handle castles differently since two pieces move
	if (moveToMake.isCastle()) {
		bitboard& friends = (color == 1) ? gameboard.state.white_pcs : gameboard.state.black_pcs;
		int king_origin = (int)util::squares::EMPTY_SQ;
		int king_dest = (int)util::squares::EMPTY_SQ;
		int rook_origin = (int)util::squares::EMPTY_SQ;
		int rook_dest = (int)util::squares::EMPTY_SQ;
		
		// set up the origin and destination squares
		switch (moveToMake.getUtilValue()) {
		case 1: // kingside
			king_origin = (color == 1) ? (int)util::squares::E1 : (int)util::squares::E8;
			king_dest = (color == 1) ? (int)util::squares::G1 : (int)util::squares::G8;
				
			rook_origin = (color == 1) ? (int)util::squares::H1 : (int)util::squares::H8;
			rook_dest = (color == 1) ? (int)util::squares::F1 : (int)util::squares::F8;
			break;
		case 2: // queenside
			king_origin = (color == 1) ? (int)util::squares::E1 : (int)util::squares::E8;
			king_dest = (color == 1) ? (int)util::squares::C1 : (int)util::squares::C8;

			rook_origin = (color == 1) ? (int)util::squares::A1 : (int)util::squares::A8;
			rook_dest = (color == 1) ? (int)util::squares::D1 : (int)util::squares::D8;
			break;
		default: // you messed up
			break;
		}
		// first move king
		gameboard.state.kings &= ~(1ULL << king_origin);
		gameboard.state.kings |= 1ULL << king_dest;
		friends &= ~(1ULL << king_origin);
		friends |= 1ULL << king_dest;

		// then move rook
		gameboard.state.rooks &= ~(1ULL << rook_origin);
		gameboard.state.rooks |= 1ULL << rook_dest;
		friends &= ~(1ULL << rook_origin);
		friends |= 1ULL << rook_dest;
	}
	else { // not a castling move
		// pointer to bitboard for this piece's and enemy's color
		bitboard* color_mask = (gameboard.state.whiteToMove) ? &gameboard.state.white_pcs : &gameboard.state.black_pcs;
		bitboard* enemy_mask = (!gameboard.state.whiteToMove) ? &gameboard.state.white_pcs : &gameboard.state.black_pcs;
		// pointer to bitboard for this piece's type
		bitboard* type_mask;
		switch (pieceID) {
		case util::Piece::PAWN:
			type_mask = &gameboard.state.pawns;
			break;
		case util::Piece::BISHOP:
			type_mask = &gameboard.state.bishops;
			break;
		case util::Piece::KNIGHT:
			type_mask = &gameboard.state.knights;
			break;
		case util::Piece::ROOK:
			type_mask = &gameboard.state.rooks;
			break;
		case util::Piece::QUEEN:
			type_mask = &gameboard.state.queens;
			break;
		case util::Piece::KING:
			type_mask = &gameboard.state.kings;
			break;
		default:
			throw "Invalid piece ID";
		}

		// clear bit for piece type/color on from square
		*color_mask &= ~(1ULL << fromSq);
		*type_mask &= ~(1ULL << fromSq);

		// clear opponent's piece on to square if this move is a capture
		if (moveToMake.isCapture()) {
			// if move is en passant, piece to clear is on a different square
			int indToClear;
			if (moveToMake.isEnPassant()) {
				indToClear = (gameboard.state.whiteToMove) ? toSq - 8 : toSq + 8;
			}
			else { // otherwise square to clear is the destination square
				indToClear = toSq;
			}

			//update castling rights if piece captured is a rook
			if ((gameboard.state.rooks >> toSq) & 1ULL) {
				if (color == -1) { // black makes the capture so white's castle rights need to be checked
					switch (toSq) {
					case 7: // white kingside
						gameboard.state.castleRights.reset(3);
						break;
					case 0: // white queenside
						gameboard.state.castleRights.reset(2);
						break;
					default:
						break;
					}
				}
				else { // white makes the capture so black's castle rights need to be checked
					switch (toSq) {
					case 63: // black kingside
						gameboard.state.castleRights.reset(1);
						break;
					case 56: // black queenside
						gameboard.state.castleRights.reset(0);
						break;
					default:
						break;
					}
				}
			}


			*enemy_mask &= ~(1ULL << indToClear); // clear enemy color
			// for now be lazy and clear all piece types at that index
			// most will already be zero
			// this will probably come back to bite me in the ass later
			gameboard.state.pawns &= ~(1ULL << indToClear);
			gameboard.state.bishops &= ~(1ULL << indToClear);
			gameboard.state.knights &= ~(1ULL << indToClear);
			gameboard.state.rooks &= ~(1ULL << indToClear);
			gameboard.state.queens &= ~(1ULL << indToClear);

			


		}

		// set bit for piece type/color on to square
		*color_mask |= 1ULL << toSq;
		*type_mask |= 1ULL << toSq;

		// if double pawn push, set en passant target square
		if (moveToMake.isEnPassant() && !moveToMake.isCapture()) {
			int epTarget = moveToMake.getUtilValue();
			gameboard.state.PIPI = util::squares(epTarget);
		}
		else { // set en passant target to empty square
			gameboard.state.PIPI = util::squares::EMPTY_SQ;
		}

		// if promotion, switch the pawn to the appropriate piece
		if (moveToMake.isPawnPromo()) {
			uint8_t promoID = moveToMake.getUtilValue();
			// clear bit from pawn bitboard
			gameboard.state.pawns &= ~(1ULL << toSq);
			// set bit on appropriate other piece bitboard
			switch (promoID) {
			case 1: // bishop
				gameboard.state.bishops |= 1ULL << toSq;
				break;
			case 2: // knight
				gameboard.state.knights |= 1ULL << toSq;
				break;
			case 3: // rook
				gameboard.state.rooks |= 1ULL << toSq;
				break;
			case 4: // queen
				gameboard.state.queens |= 1ULL << toSq;
				break;
			default:
				throw "invalid piece ID for promotion";
			}
		}

		
	}

	// handle castling rights if rook moves
	if (pieceID == util::Piece::ROOK) {
		if (gameboard.state.whiteToMove) {
			switch (fromSq)
			{
			case 0: // white queenside
				gameboard.state.castleRights.reset(2);
				break;
			case 7: // white kingside
				gameboard.state.castleRights.reset(3);
				break;
			default: // do nothing
				break;
			}
		}
		else {
			switch (fromSq)
			{
			case 56: // black queenside
				gameboard.state.castleRights.reset(0);
				break;
			case 63: // black kingside
				gameboard.state.castleRights.reset(1);
				break;
			default: // do nothing
				break;
			}
		}
	}
	else if (pieceID == util::Piece::KING) { // king move or a castle removes all future castling rights
		if (gameboard.state.whiteToMove) {
			gameboard.state.castleRights.reset(2);
			gameboard.state.castleRights.reset(3);
		}
		else {
			gameboard.state.castleRights.reset(0);
			gameboard.state.castleRights.reset(1);
		}

	}
	
	// switch side to move
	gameboard.state.whiteToMove = !gameboard.state.whiteToMove;

	// increment half/fullmove counters
	(pieceID != util::Piece::PAWN) ? gameboard.state.halfmove += 1 : gameboard.state.halfmove = 0;
	if (gameboard.state.whiteToMove) { gameboard.state.fullmove += 1; }
}

/// <summary>
/// Undo the last move (used for move generation)
/// </summary>
void DupEngine::unmakeMove() {
	gameboard.state = boardHistory[boardHistory.size()-1].state;
	boardHistory.pop_back();
	mHistory.pop_back();
}

/// <summary>
/// Print a representation of the game state to the console
/// </summary>
void DupEngine::printGameState() {
	
	char pieceStr;
	std::string castleStr;

	for (int rank = 8; rank > 0; rank--) {
		std::cout << "\n" << rank << " ";

		// first, print the board & pieces
		for (int file = (rank - 1) * 8; file < rank * 8; file++) {
			bitboard ii = 1ULL << file;
			if (gameboard.state.pawns & ii) {
				pieceStr = 'P';
			}
			else if (gameboard.state.bishops & ii) {
				pieceStr = 'B';
			}
			else if (gameboard.state.knights & ii) {
				pieceStr = 'N';
			}
			else if (gameboard.state.rooks & ii) {
				pieceStr = 'R';
			}
			else if (gameboard.state.queens & ii) {
				pieceStr = 'Q';
			}
			else if (gameboard.state.kings & ii) {
				pieceStr = 'K';
			}
			else {
				pieceStr = '.';
			}
			if (gameboard.state.black_pcs & ii) { pieceStr = tolower(pieceStr); }
			std::cout << pieceStr;
		}
		std::cout << "\t|  ";

		// print some supplemental info on lines
		switch (rank) {
		case 8:
			// print side to move
			std::cout << ((gameboard.state.whiteToMove) ? "White" : "Black") << " to move";
			break;
		case 7:
			// print en passant square
			std::cout << "En passant target square: " << util::squareStrings[(int)gameboard.state.PIPI];
			break;
		case 6:
			// print castling rights

			for (int ii = 3; ii >= 0; ii--) {
				if (gameboard.state.castleRights.test(ii)) {
					castleStr += "1";
				}
				else {
					castleStr += "0";
				}
			}
			std::cout << "Castling rights (KQkq): " << castleStr;
			break;
		case 5:
			// print half move counter
			std::cout << "Half move counter: " << gameboard.state.halfmove;
			break;
		case 4:
			// print full moon counter
			std::cout << "Half move counter: " << gameboard.state.fullmove;
			break;
		case 3:
			// print last move
			if (mHistory.size() > 0) {
				std::cout << "Last move: " << mHistory.back().getLongAN();
			}
			break;
		case 2:
			// print eval
			nn.get_active_features(gameboard);
			nn.refresh_accumulator();
			printf("Evaluation: %+.2f", nn.eval() / 100.0);
			break;
		default:
			break;
		}
	}
	std::cout << "\n  ABCDEFGH\t|\n\n";
}

/// <summary>
/// Return the number of pieces that are attacking a square
/// Find the number of attackers by calculating the possible captures from the square of interest
/// </summary>
/// <param name="square_index"></param>
/// <returns></returns>
int DupEngine::is_attacked(int square_index, int color, bitboard* color_mask) {
	std::vector<Move> atk_list;


	findPawnAttacks(atk_list, square_index, color, color_mask);
	findKnightMoves(atk_list, square_index, color, color_mask);
	findBishopMoves(atk_list, square_index, color, color_mask);
	findRookMoves(atk_list, square_index, color, color_mask);
	findQueenMoves(atk_list, square_index, color, color_mask);
	findKingMoves(atk_list, square_index, color, color_mask);

	// remove all entries that dont target square_index and/or are not captures
	for (int ii = 0; ii < atk_list.size(); ii++) {
		Move& temp = atk_list[ii];
		if (!(temp.isCapture()) || (int)temp.getFromSquare() != square_index) {
			atk_list.erase(atk_list.begin() + ii);
			ii--;
		}
		else { // is a potential capture, need to make sure the piece types match
			util::Piece move_type = temp.getPieceID();
			util::Piece target_piece = getPieceAtIndex(temp.getToSquare());
			if (move_type != target_piece) {
				atk_list.erase(atk_list.begin() + ii);
				ii--;
			}

		}
	}

	return (int)atk_list.size();
}

/// <summary>
/// return the type of piece at the given square
/// </summary>
/// <param name="sq_Index"></param>
/// <returns></returns>
util::Piece DupEngine::getPieceAtIndex(int sq_index){
	// check piece types one by one
	bool is_pawn = (gameboard.state.pawns >> sq_index) & 1ULL;
	if (is_pawn) { return util::Piece::PAWN; }

	bool is_knight = (gameboard.state.knights >> sq_index) & 1ULL;
	if (is_knight) { return util::Piece::KNIGHT; }

	bool is_bishop = (gameboard.state.bishops >> sq_index) & 1ULL;
	if (is_bishop) { return util::Piece::BISHOP; }

	bool is_rook = (gameboard.state.rooks >> sq_index) & 1ULL;
	if (is_rook) { return util::Piece::ROOK; }

	bool is_queen = (gameboard.state.queens >> sq_index) & 1ULL;
	if (is_queen) { return util::Piece::QUEEN; }

	bool is_king = (gameboard.state.kings >> sq_index) & 1ULL;
	if (is_king) { return util::Piece::KING; }

	return util::Piece::NONE;

}


/// <summary>
/// performance test for move generation
/// return the possible number of moves to the given depth
/// TODO: figure out how to time it
/// </summary>
/// <param name="depth"></param>
/// <returns></returns>
uint64_t DupEngine::perft(int depth) {
	std::vector<Move> perft_mlist;
	uint64_t nodes = 0;

	perft_mlist = getLegalMoves();
	if (depth == 0) {
		return 1;
	}
	else if (depth == 1) {
		return (uint64_t)perft_mlist.size();
	}
	else {
		int color = (gameboard.state.whiteToMove) ? 1 : -1;
		for (int ii = 0; ii < perft_mlist.size(); ii++) {
			makeMove(perft_mlist[ii]);
			if (!is_in_check(color)) {
				nodes += perft(depth - 1);
			}
			unmakeMove();
		}
	}
	return nodes;
}

/// <summary>
/// figure out if king of given color is in check
/// </summary>
/// <param name="color"></param>
/// <returns></returns>
bool DupEngine::is_in_check(int color) {
	bitboard* color_mask = (color == 1) ? &gameboard.state.white_pcs : &gameboard.state.black_pcs;
	unsigned long king_index;
	bitscanfwd_u64(&king_index,(gameboard.state.kings & *color_mask));

	return is_attacked((int)king_index, color, color_mask);
}


/// <summary>
/// given a long algebraic notation string, make the move
/// </summary>
/// <param name="LAN_move"></param>
void DupEngine::makeMoveFromString(std::string LAN_move) {
	// first, get list of legal moves
	std::vector<Move> mlist = getLegalMoves();
	bool made_move = false;
	//step through list and find the move that matches the provided string
	for (int ii = 0; ii < mlist.size(); ii++) {
		if (LAN_move.compare(mlist[ii].getLongAN()) == 0) {
			makeMove(mlist[ii]);
			made_move = true;
			break;
		}
	}

	// error, couldnt find move because string was wrong or illegal move
	if (!made_move) { std::cout << "Invalid move" << std::endl; }
}