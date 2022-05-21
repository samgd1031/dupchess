cmake_minimum_required(VERSION 3.12)


#file(GLOB HEADER_FILES CONFIGURE_DEPENDS "*.h")
#file(GLOB SRC_FILES CONFIGURE_DEPENDS "*.cpp")

#add_library(dupchess_src STATIC ${SRC_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})