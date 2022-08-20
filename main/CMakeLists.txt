set(CMAKE_CXX_STANDARD 17)

# add the exe
add_executable("${PROJECT_NAME}" "dupchess.cpp")

target_link_libraries("${PROJECT_NAME}" 
					  PUBLIC
					  dupchess_chess
					  dupchess_nnue
					  nnue_weights
)

