# add the exe
add_executable("${PROJECT_NAME}" "dupchess.cpp")

target_link_libraries("${PROJECT_NAME}" 
					  PUBLIC
					  dupchess_chess
					  dupchess_nnue
)

# copy the weights file to the build directory
add_custom_target(weights ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${WEIGHT_FILE})
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${WEIGHT_FILE}
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/${WEIGHT_FILE}
		${CMAKE_CURRENT_BINARY_DIR}/${WEIGHT_FILE}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${WEIGHT_FILE}
)