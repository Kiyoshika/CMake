# C Matrix
This is a simple matrix / linear algebra library I'm writing in pure C for my own learning experience. This is far from optimal to use in speed-critical settings as some of these implementations are pretty naive (e.g., matrix multiplication)

This includes `Matrix.h` and `Vector.h`. Keep in mind the `Vector` implementation is statically-sized and not your typical "dynamic" vector.

# Structure
This is a wrapper around a `float*` where offsets are computed to "mimic" a 2D array. 

# Supported Operations
The basic matrix operations are supported such as transpose, multiply, element-wise operations between matrices and/or scalars, apply functions, etc.

Some functions have a "copy" variant (new Matrix created) or an "inplace" variant where no new Matrix is allocated. This is mainly for situations where you are doing a lot of repeated operations (e.g. multiplication or transpose) and want to avoid heap fragmentation from thousands of allocations.

# Parallelization
This library optionally uses OpenMP if you want to speed up matrix multiplication.

If you want to use multiple threads, use `cmake -DUSE_PARALLEL=ON ..`

NOTE: if you are on Windows using MinGW, you must use MinGW's installer to install `mingw-pthreads-w32-...` libraries.

# Building from Source
* `git clone https://github.com/Kiyoshika/CMatrix`
* `cd CMatrix`
* `mkdir build && cd build`
* `cmake ..`
* `make`

This will create a static library `libmatrix.a` which you can include into your projects, or (recommended) follow the below section instructions.

# Linking to Other Projects
The easiest way is to probably just `git submodule add https://github.com/Kiyoshika/CMatrix` into your project and in your CMake file
include a `add_subdirectory(path/to/CMatrix)` which will include CMatrix as part of your build.

Then in your pieces of code that depend on it, you can use `target_link_libraries([your target] matrix)`

If it can't find the headers even after linking, you can use `target_include_directories([your target] PUBLIC ${CMatrix_SOURCE_DIR}/include/matrix)`

If you don't use CMake, you can build from source (above section) and manually link the static library.

# Benchmarks
As mentioned, the matrix multiplication is a fairly naive implementation and would like to improve it someday.

In the meantime, below are some benchmarks for `mat_multiply`

Specs:
* Processor: Intel i7 9th gen
* Optimization: O2
* Compiler: gcc
* Cores: 6

Results (No Parallelization):
* (512, 512) x (512, 512)     - around 1 second
* (1024, 1024) x (1024, 1024) - around 2 seconds
* (2048, 2048) x (2048, 2048) - around 10 seconds
* (4096, 4096) x (4096, 4096) - around 75 seconds
* (10000, 10) x (10, 10000)   - around 2 seconds
* (10, 10000) x (10000, 10)   - under 1 second

Results (Parallelization):
* (512, 512) x (512, 512)     - under 1 second
* (1024, 1024) x (1024, 1024) - under 1 second
* (2048, 2048) x (2048, 2048) - under 1 second
* (4096, 4096) x (4096, 4096) - around 2 seconds
* (10000, 10) x (10, 10000)   - around 2 seconds
* (10, 10000) x (10000, 10)   - under 1 second

Seems that with parallelization (and up to 6 cores) our scenarios are (almost) \<= 1 second.
