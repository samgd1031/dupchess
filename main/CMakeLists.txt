cmake_minimum_required(VERSION 3.12)

# add the exe
add_executable("${PROJECT_NAME}" "dupchess.cpp")

target_link_libraries("${PROJECT_NAME}" dupchess_chess)
target_link_libraries("${PROJECT_NAME}" dupchess_nnue)
#target_link_libraries("${PROJECT_NAME}" dupchess_nnue_train)