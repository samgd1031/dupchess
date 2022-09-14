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
	ASSERT_EQ(eng.perft(5), 4865609);
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
/*
	// short castling gives check:
	eng.gameboard.setBoardFromFEN("5k2/8/8/8/8/8/8/4K2R w K - 0 1");
	EXPECT_EQ(eng.perft(6), 661072);
	eng.gameboard.setBoardFromFEN("4k2r/8/8/8/8/8/8/5K2 b k - 0 1");
	EXPECT_EQ(eng.perft(6), 661072);

	// long castling gives check:
	eng.gameboard.setBoardFromFEN("3k4/8/8/8/8/8/8/R3K3 w Q - 0 1");
	EXPECT_EQ(eng.perft(6), 803711);
	eng.gameboard.setBoardFromFEN("r3k3/8/8/8/8/8/8/3K4 b q - 0 1");
	EXPECT_EQ(eng.perft(6), 803711);

	// castling (including losing cr due to rook capture):
	eng.gameboard.setBoardFromFEN("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1");
	EXPECT_EQ(eng.perft(6), 1274206);
	eng.gameboard.setBoardFromFEN("r3k2r/7b/8/8/8/8/1B4BQ/R3K2R b KQkq - 0 1");
	EXPECT_EQ(eng.perft(6), 1274206);
}*/