#include <gtest/gtest.h>
#include "../src/nnue/nnue.h"
#include "../src/chess/utils.h"

// tests to check NNUE functionality
TEST(NnueTest, halfKP_indices) {
	std::string weight = "../main/" + (std::string)WEIGHTFILE;
	NNUE nn = NNUE(weight);
	Board b = Board();

	b.setBoardFromFEN("7k/8/8/8/8/8/8/KP6 w - - 0 1");
	nn.get_active_features(b);
	nn.refresh_accumulator();
	std::cout << nn.eval() << "\n";
	
}