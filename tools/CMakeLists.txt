cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add the exe
add_executable("${PROJECT_NAME}_tools" "bin_proc.cpp")
