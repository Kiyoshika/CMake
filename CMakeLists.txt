# CMakeList.txt : CMake project for CMatrix, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("CMatrix" C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall")

# Add source to this project's executable.
add_subdirectory(src/main)