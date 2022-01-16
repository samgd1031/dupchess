#pragma once


// bitboard/hash (64 bit integer)
typedef long long bitboard;
typedef long long int64;

// square ID
enum squareID = {A1, B1, C1, D1, E1, F1, G1, H1,
				 A2, B2, C2, D2, E2, F2, G2, H2,
				 A3, B3, C3, D3, E3, F3, G3, H3,
				 A4, B4, C4, D4, E4, F4, G4, H4,
				 A5, B5, C5, D5, E5, F5, G5, H5,
				 A6, B6, C6, D6, E6, F6, G6, H6,
				 A7, B7, C7, D7, E7, F7, G7, H7,
				 A8, B8, C8, D8, E8, F8, G8, H8, ER};

// allow for incrementing on square ID
inline squareID operator++ (squareID& sq) { return (squareID)(sq + 1); }
inline squareID operator++ (squareID& sq, int) 
	{ squareID old = sq; sq = (squareID)(sq + 1); return old }


// piece ID
enum pieceID = { EMPTY, W_P, W_N, W_B, W_R, W_Q, W_K,
				B_P, B_N, B_B, B_R, B_Q, B_K, INVALID };

// allow for incrementing on piece ID
inline pieceID operator++ (pieceID& pic { return (pieceID)(pic + 1); }
inline pieceID operator++ (pieceID& pic, int)
{
	pieceID old = pic; pic = (pieceID)(pic + 1); return old
}

// colors
enum color = { WHITE, BLACK, NONE };

// struct to hold all bitboards needed for engine operation
struct boardBBs
{
	//piece information per square
	squareID squares[H8 + 2];
	//bitboards for each piece
	bitboard pieces[B_K + 2];
	//other bitboards to streamline engine operations
	bitboard emptySq;				// empty squares
	bitboard occupidedSq;			// occupied squares
	bitboard colorPcs[BLACK + 1];	// all pieces of a color
};

// struct to hold the entire board state
struct boardState
{
	int64 hash;  // still need to figure out exactly why hashes are useful
	bool castleRights[2];  // true if can castle, ind0 = white, ind1 = black
	squareID PIPIsquare; // en passant square
	bool inCheck;
	color toMove;  // side to move (white/black)
	int repetitions;
};