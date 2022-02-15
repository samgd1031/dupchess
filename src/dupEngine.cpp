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
// captures
// /////////////////////
// TODO: promotions
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

	// pawn attack targets are the squares to the northeast (southwest) and northwest (southeast) for white/black
	// to avoid A/H file pawns the appropriate rank will be masked out
	// space must also be occupied (not empty) to be a valid attack target
	bitboard east_mask = (color == 1) ? 0x7F7F7F7F7F7F7F7F : 0xFEFEFEFEFEFEFEFE;
	bitboard west_mask = (color == 1) ? 0xFEFEFEFEFEFEFEFE : 0x7F7F7F7F7F7F7F7F;
	
	bitboard east_atk = util::genShift((gameboard.current_state.pawns & *color_mask) & east_mask, color * 9) & ~empty;
	bitboard west_atk = util::genShift((gameboard.current_state.pawns & *color_mask) & west_mask, color * 7) & ~empty;


	// encode single moves and add to move list
	int movecount = std::_Popcount(single_tgts);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanReverse64(&target_ind, single_tgts);
		mlist.push_back(Move((uint32_t)(target_ind - 8 * color), (uint32_t)target_ind, false, false, false));
		single_tgts ^= 1ULL << target_ind;
	}

	// encode double moves and add to move list
	movecount = std::_Popcount(dbl_tgts);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanReverse64(&target_ind, dbl_tgts);
		mlist.push_back(Move((uint32_t)(target_ind - 16 * color), (uint32_t)target_ind, false, false, true));
		dbl_tgts ^= 1ULL << target_ind;
	}

	// encode captures and add to move list
	movecount = std::_Popcount(east_atk);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanReverse64(&target_ind, east_atk);
		mlist.push_back(Move((uint32_t)(target_ind - 9 * color), (uint32_t)target_ind, true, false, false));
		east_atk ^= 1ULL << target_ind;
	}
	
	movecount = std::_Popcount(west_atk);
	for (int ii = 0; ii < movecount; ii++) {
		unsigned long target_ind;
		_BitScanReverse64(&target_ind, west_atk);
		mlist.push_back(Move((uint32_t)(target_ind - 7 * color), (uint32_t)target_ind, true, false, false));
		west_atk ^= 1ULL << target_ind;
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
		printf("%s has no moves!\n", (gameboard.current_state.whiteToMove) ? "White" : "Black");
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
	bitboard* color_mask = (gameboard.current_state.whiteToMove) ? &gameboard.current_state.white_pcs : &gameboard.current_state.black_pcs;
	bitboard* enemy_mask = (!gameboard.current_state.whiteToMove) ? &gameboard.current_state.white_pcs : &gameboard.current_state.black_pcs;
	// pointer to bitboard for this piece's type
	bitboard* type_mask;
	switch (pieceID) {
	case 0:
		type_mask = &gameboard.current_state.pawns;
		break;
	case 1:
		type_mask = &gameboard.current_state.bishops;
		break;
	case 2:
		type_mask = &gameboard.current_state.knights;
		break;
	case 3:
		type_mask = &gameboard.current_state.rooks;
		break;
	case 4:
		type_mask = &gameboard.current_state.queens;
		break;
	case 5:
		type_mask = &gameboard.current_state.kings;
		break;
	default:
		throw "Invalid piece ID";
	}

	// clear bit for piece type/color on from square
	*color_mask &= ~(1ULL << fromSq);
	*type_mask &= ~(1ULL << fromSq);

	// clear opponent's piece on to square if this move is a capture
	if (moveToMake.isCapture()) {
		*enemy_mask &= ~(1ULL << toSq); // clear enemy color
		// for now be lazy and clear all piece types at that index
		// most will already be zero
		// this will probably come back to bite me in the ass later
		gameboard.current_state.pawns &= ~(1ULL << toSq); 
		gameboard.current_state.bishops &= ~(1ULL << toSq); 
		gameboard.current_state.knights &= ~(1ULL << toSq); 
		gameboard.current_state.rooks &= ~(1ULL << toSq); 
		gameboard.current_state.queens &= ~(1ULL << toSq);

	}

	// set bit for piece type/color on to square
	*color_mask |= 1ULL << toSq;
	*type_mask |= 1ULL << toSq;

	// if double pawn push, set en passant target square
	if (moveToMake.isDoublePawnPush()) {
		int epTarget = moveToMake.getUtilValue();
		gameboard.current_state.PIPI = util::squares(epTarget);
	}
	else { // set en passant target to empty square
		gameboard.current_state.PIPI = util::squares(64);
	}
	
	// switch side to move
	gameboard.current_state.whiteToMove = !gameboard.current_state.whiteToMove;

	// increment half/fullmove counters
	(pieceID != 0) ? gameboard.current_state.halfmove += 1 : gameboard.current_state.halfmove = 0;
	if (gameboard.current_state.whiteToMove) { gameboard.current_state.fullmove += 1; }
}

/// <summary>
/// Print a representation of the game state
/// </summary>
void DupEngine::printGameState() {
	// first, print the board
	uint64_t ii = 1ULL << 63;
	char pieceStr;

	for (int count = 64; count > 0; count--) {
		if (count % 8 == 0) {
			printf("\n%i ", count / 8);
		}
		if (gameboard.current_state.pawns & ii) {
			pieceStr = 'P';
		}
		else if (gameboard.current_state.bishops & ii) {
			pieceStr = 'B';
		}
		else if (gameboard.current_state.knights & ii) {
			pieceStr = 'N';
		}
		else if (gameboard.current_state.rooks & ii) {
			pieceStr = 'R';
		}
		else if (gameboard.current_state.queens & ii) {
			pieceStr = 'Q';
		}
		else if (gameboard.current_state.kings & ii) {
			pieceStr = 'K';
		}
		else {
			pieceStr = '.';
		}
		if (gameboard.current_state.black_pcs & ii) { pieceStr = tolower(pieceStr); }
		printf("%c", pieceStr);

		ii >>= 1;
	}
	printf("\n  ABCDEFGH\n\n");

	// print side to move
	printf("%s to move\n", (gameboard.current_state.whiteToMove) ? "White" : "Black");

	// print en passant square
	printf("En passant target square: %s\n", util::squareStrings[(int)gameboard.current_state.PIPI]);

	// print castling rights
	std::string castleStr;
	for (int ii = 3; ii >= 0; ii--) {
		if (gameboard.current_state.castleRights.test(ii)) {
			castleStr += "1";
		}
		else {
			castleStr += "0";
		}
	}
	printf("Castling rights (KQkq): %s\n", castleStr.c_str());

	// print half/full move counters
	printf("Half move counter: %i\n", gameboard.current_state.halfmove);
	printf("Full move counter: %i\n", gameboard.current_state.fullmove);
}