cmake_minimum_required(VERSION 3.12)

add_subdirectory(chess)
add_subdirectory(nnue)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})