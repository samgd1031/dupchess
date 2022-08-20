enable_testing()

add_executable(dupchess_tests
				tests.cpp
)

target_link_libraries(
	dupchess_tests
	dupchess_chess
	GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(dupchess_tests)