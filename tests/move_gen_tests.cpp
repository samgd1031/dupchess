#include <gtest/gtest.h>
#include "../src/chess/dupEngine.h"

// series of perft tests to check move generation
TEST(MoveGen, perft) {
	DupEngine eng = DupEngine();
	// from initial position
	ASSERT_EQ(eng.perft(0), 1);
	ASSERT_EQ(eng.perft(1), 20);
	ASSERT_EQ(eng.perft(2), 400);
	ASSERT_EQ(eng.perft(3), 8902);
	ASSERT_EQ(eng.perft(4), 197281);
	//ASSERT_EQ(eng.perft(5), 4865609);
}

/////// edge cases from http://www.talkchess.com/forum3/viewtopic.php?t=47318
TEST(MoveGen, illegalEP){
	DupEngine eng = DupEngine();
	// avoid illegal en passant capture:
	eng.gameboard.setBoardFromFEN("8/5bk1/8/2Pp4/8/1K6/8/8 w - d6 0 1");
	EXPECT_EQ(eng.perft(6), 824064);
	eng.gameboard.setBoardFromFEN("8/8/1k6/8/2pP4/8/5BK1/8 b - d3 0 1");
	EXPECT_EQ(eng.perft(6), 824064);
}

TEST(MoveGen, EPCheck) {
	DupEngine eng = DupEngine();
	// en passant capture checks opponent
	eng.gameboard.setBoardFromFEN("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1");
	EXPECT_EQ(eng.perft(6), 1440467);
	eng.gameboard.setBoardFromFEN("8/5k2/8/2Pp4/2B5/1K6/8/8 w - d6 0 1");
	EXPECT_EQ(eng.perft(6), 1440467);
}

TEST(MoveGen, ShortCastleCheck) {
	DupEngine eng = DupEngine();
	// short castling gives check:
	eng.gameboard.setBoardFromFEN("5k2/8/8/8/8/8/8/4K2R w K - 0 1");
	EXPECT_EQ(eng.perft(6), 661072);
	eng.gameboard.setBoardFromFEN("4k2r/8/8/8/8/8/8/5K2 b k - 0 1");
	EXPECT_EQ(eng.perft(6), 661072);
}

TEST(MoveGen, LongCastleCheck) {
	DupEngine eng = DupEngine();
	// long castling gives check:
	eng.gameboard.setBoardFromFEN("3k4/8/8/8/8/8/8/R3K3 w Q - 0 1");
	EXPECT_EQ(eng.perft(6), 803711);
	eng.gameboard.setBoardFromFEN("r3k3/8/8/8/8/8/8/3K4 b q - 0 1");
	EXPECT_EQ(eng.perft(6), 803711);
}

TEST(MoveGen, Castles) {
	DupEngine eng = DupEngine();
	// castling (including losing cr due to rook capture):
	eng.gameboard.setBoardFromFEN("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1");
	EXPECT_EQ(eng.perft(4), 1274206);
	eng.gameboard.setBoardFromFEN("r3k2r/7b/8/8/8/8/1B4BQ/R3K2R b KQkq - 0 1");
	EXPECT_EQ(eng.perft(4), 1274206);
}

TEST(MoveGen, CastlePrevented) {
	DupEngine eng = DupEngine();
	// castling prevented
	eng.gameboard.setBoardFromFEN("r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1");
	EXPECT_EQ(eng.perft(4), 1720476);
	eng.gameboard.setBoardFromFEN("r3k2r/8/5Q2/8/8/3q4/8/R3K2R w KQkq - 0 1");
	EXPECT_EQ(eng.perft(4), 1720476);
}

TEST(MoveGen, PromoteOutOfCheck) {
	DupEngine eng = DupEngine();
	// promoting out of check
	eng.gameboard.setBoardFromFEN("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1");
	EXPECT_EQ(eng.perft(6), 3821001);
	eng.gameboard.setBoardFromFEN("3K4/8/8/8/8/8/4p3/2k2R2 b - - 0 1");
	EXPECT_EQ(eng.perft(6), 3821001);
}

TEST(MoveGen, DiscoveredCheck) {
	DupEngine eng = DupEngine();
	// discovered check
	eng.gameboard.setBoardFromFEN("8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1");
	EXPECT_EQ(eng.perft(5), 1004658);
	eng.gameboard.setBoardFromFEN("5K2/8/1Q6/2N5/8/1p2k3/8/8 w - - 0 1");
	EXPECT_EQ(eng.perft(5), 1004658);
}

TEST(MoveGen, PromoteCheck) {
	DupEngine eng = DupEngine();
	// promoting to give check
	eng.gameboard.setBoardFromFEN("4k3/1P6/8/8/8/8/K7/8 w - - 0 1");
	EXPECT_EQ(eng.perft(6), 217342);
	eng.gameboard.setBoardFromFEN("8/k7/8/8/8/8/1p6/4K3 b - - 0 1");
	EXPECT_EQ(eng.perft(6), 217342);
}

TEST(MoveGen, UnderpromoteCheck) {
	DupEngine eng = DupEngine();
	// Underpromoting to give check
	eng.gameboard.setBoardFromFEN("8/P1k5/K7/8/8/8/8/8 w - - 0 1");
	EXPECT_EQ(eng.perft(6), 92683);
	eng.gameboard.setBoardFromFEN("8/8/8/8/8/k7/p1K5/8 b - - 0 1");
	EXPECT_EQ(eng.perft(6), 92683);
}

TEST(MoveGen, SelfStalemate) {
	DupEngine eng = DupEngine();
	// self stalemate
	eng.gameboard.setBoardFromFEN("K1k5/8/P7/8/8/8/8/8 w - - 0 1");
	EXPECT_EQ(eng.perft(6), 2217);
	eng.gameboard.setBoardFromFEN("8/8/8/8/8/p7/8/k1K5 b - - 0 1");
	EXPECT_EQ(eng.perft(6), 2217);
}

TEST(MoveGen, StalemateCheckmate) {
	DupEngine eng = DupEngine();
	// stalemate/checkmate
	eng.gameboard.setBoardFromFEN("8/k1P5/8/1K6/8/8/8/8 w - - 0 1");
	EXPECT_EQ(eng.perft(7), 567584);
	eng.gameboard.setBoardFromFEN("8/8/8/8/1k6/8/K1p5/8 b - - 0 1");
	EXPECT_EQ(eng.perft(7), 567584);
}

TEST(MoveGen, DoubleCheck) {
	DupEngine eng = DupEngine();
	// double check
	eng.gameboard.setBoardFromFEN("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1");
	EXPECT_EQ(eng.perft(4), 23527);
	eng.gameboard.setBoardFromFEN("8/5k2/8/5N2/5Q2/2K5/8/8 w - - 0 1");
	EXPECT_EQ(eng.perft(4), 23527);
}