cmake_minimum_required(VERSION 3.14)

# set the project name
project("dupchess")

#dependencies
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG e2239ee6043f73722e7aa812a459f54a28552929  # release 1.11.0
)

FetchContent_Declare(
	CMakeRC
	GIT_REPOSITORY https://github.com/vector-of-bool/cmrc
	GIT_TAG 9a80e17c75fca503a55a38ea011dcfaf2a830cc9  # release 2.0.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest CMakeRC)

enable_testing()

add_subdirectory("main")
add_subdirectory("src")
add_subdirectory("tests")
add_subdirectory("res")


