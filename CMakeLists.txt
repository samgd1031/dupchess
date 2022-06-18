cmake_minimum_required(VERSION 3.12)

# set the project name
project("dupchess")

add_subdirectory("main")
add_subdirectory("src")
add_subdirectory("tests")

include_directories("src")