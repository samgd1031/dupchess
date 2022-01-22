cmake_minimum_required(VERSION "3.20.0")

# set the project name
project("DupChess" VERSION "0.1")

# add the exe
add_executable("${PROJECT_NAME}" "dupchess.cpp")


install(TARGETS "${PROJECT_NAME}" DESTINATION bin)	#${CMAKE_INSTALL_PREFIX}/bin
install(FILES "dupchess.cpp" DESTINATION src)		#${CMAKE_INSTALL_PREFIX}/src