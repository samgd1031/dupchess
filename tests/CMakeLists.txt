enable_testing()

add_executable(MoveGen
				tests.cpp
)

target_link_libraries(
	MoveGen
	dupchess_chess
	GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(MoveGen)