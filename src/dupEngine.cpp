#include "dupEngine.h"


// Constructor ////////////////////////////
DupEngine::DupEngine(void) {
	gameboard = Board();
	mlist.reserve(200);
	mHistory.reserve(80);
	boardHistory.reserve(40);
	gameboard.setBoardFromFEN(DupEngine::START_FEN);
	srand((int)time(NULL));  // set random generator seed
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
	std::string lines[4] = { "Up yours, Jonah!\n",
							 "Bite my shiny metal ass!\n",
							 "I'm afraid I can't let you do that...\n",
							 "'Not only do I smell bad, I'm horrible at chess!'\n\t-Jonah"};

	return lines[std::rand() % 4];
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

	std::vector<Move> movelist;
	// pawns
	DupEngine::findPawnMoves(movelist, color, color_mask);

	// rooks
	bitboard rooks_to_move = gameboard.state.rooks & *color_mask;
	int n_rooks = std::_Popcount(rooks_to_move);
	for (int ii = 0; ii < n_rooks; ii++) {
		unsigned long rook_index;
		_BitScanForward64(&rook_index, rooks_to_move);
		DupEngine::findRookMoves(movelist, rook_index, color, color_mask);
		rooks_to_move &= ~(1ULL << rook_index);
	}

	// bishops
	bitboard bish_to_move = gameboard.state.bishops & *color_mask;
	int n_bish = std::_Popcount(bish_to_move);
	for (int ii = 0; ii < n_bish; ii++) {
		unsigned long bish_index;
		_BitScanForward64(&bish_index, bish_to_move);
		DupEngine::findBishopMoves(movelist, bish_index, color, color_mask);
		bish_to_move &= ~(1ULL << bish_index);
	}
	

	return movelist;
}

// return a move list of all legal pawn moves
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
	int movecount = std::_Popcount(single_tgts);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanForward64(&target_ind, single_tgts);
		// if pawn makes it to the end add all possible promotion moves
		if(((color == 1) & (target_ind / 8 == 7)) | ((color == -1) & (target_ind / 8 == 0))){
			for (uint8_t jj = 1; jj < 5; jj++) {
				mlist.push_back(Move((uint32_t)(target_ind - 8 * color), (uint32_t)target_ind, util::Piece::PAWN, false, true, false, jj));
			}
		}
		else{ // no promotion
			mlist.push_back(Move((uint32_t)(target_ind - 8 * color), (uint32_t)target_ind, util::Piece::PAWN, false, false, false, 0));
		}
		single_tgts ^= 1ULL << target_ind;
	}

	// encode double moves and add to move list
	movecount = std::_Popcount(dbl_tgts);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanForward64(&target_ind, dbl_tgts);
		mlist.push_back(Move((uint32_t)(target_ind - 16 * color), (uint32_t)target_ind, util::Piece::PAWN, false, false, true, 0));
		dbl_tgts ^= 1ULL << target_ind;
	}

	// encode captures and add to move list
	movecount = std::_Popcount(east_atk);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanForward64(&target_ind, east_atk);
		// if pawn makes it to the end add all possible promotion moves
		if (((color == 1) & (target_ind / 8 == 7)) | ((color == -1) & (target_ind / 8 == 0))) {
			for (uint8_t jj = 1; jj < 5; jj++) {
				mlist.push_back(Move((uint32_t)(target_ind - 9 * color), (uint32_t)target_ind, util::Piece::PAWN, true, true, false, jj));
			}
		}
		else{
			mlist.push_back(Move((uint32_t)(target_ind - 9 * color), (uint32_t)target_ind, util::Piece::PAWN, true, false, false, 0));
		}
		east_atk ^= 1ULL << target_ind;
	}
	
	movecount = std::_Popcount(west_atk);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanForward64(&target_ind, west_atk);
		// if pawn makes it to the end add all possible promotion moves
		if (((color == 1) & (target_ind / 8 == 7)) | ((color == -1) & (target_ind / 8 == 0))) {
			for (uint8_t jj = 1; jj < 5; jj++) {
				mlist.push_back(Move((uint32_t)(target_ind - 7 * color), (uint32_t)target_ind, util::Piece::PAWN, true, true, false, jj));
			}
		}
		else{
			mlist.push_back(Move((uint32_t)(target_ind - 7 * color), (uint32_t)target_ind, util::Piece::PAWN, true, false, false, 0));
		}
		west_atk ^= 1ULL << target_ind;
	}

	// check en passant
	if (gameboard.state.PIPI != util::squares::EMPTY_SQ) {
		bitboard epTarget = 1ULL << (int)gameboard.state.PIPI;

		bitboard east_atk = util::genShift((gameboard.state.pawns & *color_mask) & east_mask, color * 9) & epTarget;
		bitboard west_atk = util::genShift((gameboard.state.pawns & *color_mask) & west_mask, color * 7) & epTarget;

		// encode moves
		movecount = std::_Popcount(east_atk);
		for (int ii = 0; ii < movecount; ii++) {
			unsigned long target_ind;
			_BitScanForward64(&target_ind, east_atk);
			mlist.push_back(Move((uint32_t)(target_ind - 9 * color), (uint32_t)target_ind, util::Piece::PAWN, true, false, true, 0));
			east_atk ^= 1ULL << target_ind;
		}

		movecount = std::_Popcount(west_atk);
		for (int ii = 0; ii < movecount; ii++) {
			unsigned long target_ind;
			_BitScanForward64(&target_ind, west_atk);
			mlist.push_back(Move((uint32_t)(target_ind - 7 * color), (uint32_t)target_ind, util::Piece::PAWN, true, false, true, 0));
			east_atk ^= 1ULL << target_ind;
		}
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
	int n_moves = std::_Popcount(rook_moves);
	for (int ii = 0; ii < n_moves; ii++) {
		unsigned long target_ind;
		_BitScanForward64(&target_ind, rook_moves);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::ROOK, false, false, false, 0));
		rook_moves ^= 1ULL << target_ind;
	}

	// encode captures
	n_moves = std::_Popcount(rook_atks);
	for (int ii = 0; ii < n_moves; ii++) {
		unsigned long target_ind;
		_BitScanForward64(&target_ind, rook_atks);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::ROOK, true, false, false, 0));
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
	bitboard bish_moves = (diag | anti_diag) & ~occ;

	// get attacks (collisions with pieces of opposite color)
	bitboard bish_atks = (diag | anti_diag) & (occ & ~*color_mask);

	// encode moves
	int n_moves = std::_Popcount(bish_moves);
	for (int ii = 0; ii < n_moves; ii++) {
		unsigned long target_ind;
		_BitScanForward64(&target_ind, bish_moves);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::BISHOP, false, false, false, 0));
		bish_moves ^= 1ULL << target_ind;
	}

	// encode captures
	n_moves = std::_Popcount(bish_atks);
	for (int ii = 0; ii < n_moves; ii++) {
		unsigned long target_ind;
		_BitScanForward64(&target_ind, bish_atks);
		mlist.push_back(Move((uint32_t)sqIndex, (uint32_t)target_ind, util::Piece::BISHOP, true, false, false, 0));
		bish_atks ^= 1ULL << target_ind;
	}
}

/// <makeMove>
/// Take the current list of moves and make one
/// for now will be a random move, eventually some smart eval stuff will need to happen
/// </makeMove>
void DupEngine::makeMove() {
	// get list of all legal moves
	mlist = getLegalMoves();

	// pick a random move from movelist
	if (mlist.size() == 0) {
		printf("%s has no moves!\n", (gameboard.state.whiteToMove) ? "White" : "Black");
		return;
	}
	Move moveToMake = mlist[std::rand() % mlist.size()];
	printf("Making Move: %s\n", moveToMake.getLongSAN().c_str());

	// process the move and update game state accordingly
	boardHistory.push_back(gameboard); // save state to history before making changes
	mHistory.push_back(moveToMake);

	// get piece that's moving, origin and destination squares
	int pieceID = moveToMake.getPieceID();
	int fromSq = moveToMake.getFromSquare();
	int toSq = moveToMake.getToSquare();
	
	// pointer to bitboard for this piece's and enemy's color
	bitboard* color_mask = (gameboard.state.whiteToMove) ? &gameboard.state.white_pcs : &gameboard.state.black_pcs;
	bitboard* enemy_mask = (!gameboard.state.whiteToMove) ? &gameboard.state.white_pcs : &gameboard.state.black_pcs;
	// pointer to bitboard for this piece's type
	bitboard* type_mask;
	switch (pieceID) {
	case 0:
		type_mask = &gameboard.state.pawns;
		break;
	case 1:
		type_mask = &gameboard.state.bishops;
		break;
	case 2:
		type_mask = &gameboard.state.knights;
		break;
	case 3:
		type_mask = &gameboard.state.rooks;
		break;
	case 4:
		type_mask = &gameboard.state.queens;
		break;
	case 5:
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
		if(moveToMake.isEnPassant()){
			indToClear = (gameboard.state.whiteToMove) ? toSq - 8 : toSq + 8 ;
		}
		else{ // otherwise square to clear is the destination square
			indToClear = toSq;
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
		switch(promoID){
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
	
	// switch side to move
	gameboard.state.whiteToMove = !gameboard.state.whiteToMove;

	// increment half/fullmove counters
	(pieceID != 0) ? gameboard.state.halfmove += 1 : gameboard.state.halfmove = 0;
	if (gameboard.state.whiteToMove) { gameboard.state.fullmove += 1; }
}

/// <summary>
/// Print a representation of the game state
/// </summary>
void DupEngine::printGameState() {
	// first, print the board
	char pieceStr;

	for (int rank = 8; rank > 0; rank--) {
		printf("\n%i ", rank);
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
			printf("%c", pieceStr);
		}
	}
	printf("\n  ABCDEFGH\n\n");

	// print side to move
	printf("%s to move\n", (gameboard.state.whiteToMove) ? "White" : "Black");

	// print en passant square
	printf("En passant target square: %s\n", util::squareStrings[(int)gameboard.state.PIPI]);

	// print castling rights
	std::string castleStr;
	for (int ii = 3; ii >= 0; ii--) {
		if (gameboard.state.castleRights.test(ii)) {
			castleStr += "1";
		}
		else {
			castleStr += "0";
		}
	}
	printf("Castling rights (KQkq): %s\n", castleStr.c_str());

	// print half/full move counters
	printf("Half move counter: %i\n", gameboard.state.halfmove);
	printf("Full move counter: %i\n", gameboard.state.fullmove);
}