# C Matrix
This is a simple matrix / linear algebra library I'm writing in pure C for my own learning experience. This is far from optimal to use in speed-critical settings as some of these implementations are pretty naive (e.g., matrix multiplication)

## Benchmarks
I will soon be working on improving the speed of matrix multiplication. Here are some current benchmarks:

* (512, 512) x (512, 512)     - under 1 second (x86 Release mode)
* (1024, 1024) x (1024, 1024) - under 1 seconds (x86 Release mode)
* (2048, 2048) x (2048, 2048) - around 13 seconds (x86 Release mode)
* (4096, 4096) x (4096, 4096) - around 100 seconds
* (10000, 10) x (10, 10000)   - around 3 seconds (x86 Release mode)