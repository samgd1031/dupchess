cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)

# set the project name
project("dupchess")

# name of weight file for referencing elsewhere in build
set(WEIGHT_FILE "velma_v1.wgt")

#dependencies
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG e2239ee6043f73722e7aa812a459f54a28552929  # release 1.11.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory("main")
add_subdirectory("src")
add_subdirectory("tests")


