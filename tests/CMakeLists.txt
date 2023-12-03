enable_testing()

add_executable(Tests
				move_gen_tests.cpp
				nnue_tests.cpp
)

target_link_libraries(
	Tests
	dupchess_chess
	dupchess_nnue
	GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Tests)
target_compile_definitions(Tests PRIVATE WEIGHTFILE="${WEIGHT_FILE}")