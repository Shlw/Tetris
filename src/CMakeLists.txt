set(CMAKE_CXX_STANDARD 11)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
ADD_EXECUTABLE(tetris tetris.cpp jsoncpp.cpp)
