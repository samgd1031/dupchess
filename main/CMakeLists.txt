cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)

# add the exe
add_executable("${PROJECT_NAME}" "dupchess.cpp")

target_link_libraries("${PROJECT_NAME}" 
					  dupchess_chess
					  dupchess_nnue
)