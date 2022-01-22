cmake_minimum_required(VERSION "3.20.0")

# set the project name
project("DupChess")

# add the exe
add_executable("${PROJECT_NAME}" "dupchess.cpp")

add_subdirectory("include")
target_link_libraries("${PROJECT_NAME}" "adder")