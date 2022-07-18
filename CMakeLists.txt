# CMakeList.txt : CMake project for CMatrix, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("CMatrix" C)

option(USE_PARALLEL "Use openmp for parallelization" OFF)

set(CMAKE_C_STANDARD 99)

if (USE_PARALLEL)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -fopenmp -Wall -g")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -g")
endif()

# Add source to this project's executable.
add_subdirectory(src/main)
