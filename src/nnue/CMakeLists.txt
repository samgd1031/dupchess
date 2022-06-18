cmake_minimum_required(VERSION 3.12)


#file(GLOB HEADER_FILES CONFIGURE_DEPENDS "*.h")
file(GLOB SRC_FILES CONFIGURE_DEPENDS "*.cpp")

add_library(dupchess_nnue STATIC ${SRC_FILES} ${HEADER_FILES})