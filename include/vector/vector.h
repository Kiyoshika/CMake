#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct Vector
{
	float* data;
	size_t n_elem;
} Vector;

// initialize vector of size n_elem (default to 0.0)
void vec_init(Vector** vec, const size_t n_elem);

// create vector from float pointer of data
Vector* vec_create(const float* data, const size_t n_elem);

// create copy of vector
Vector* vec_copy(const Vector* vec);

// get value at index i
float vec_at(const Vector* vec, const size_t i);

// set value at index i
void vec_set(Vector** vec, const size_t i, const float value);

// dot product between two vectors
float vec_dot(const Vector* vec1, const Vector* vec2);

// fill vector with random values between lower_bound and upper_bound
void vec_random(Vector** vec, const float lower_bound, const float upper_bound);

// fill vector with constant value
void vec_fill(Vector** vec, const float value);

// apply function to each element in vector using function pointer. function pointer uses argv if user needs to pass any additional parameters to the apply function, otherwise can pass NULL. value returned from function will be set in the vector's cell.
void vec_apply(Vector** vec, float (*apply_func)(float x, float* argv), float* argv);

// free memory allocated by vector
void vec_free(Vector** vec);

#endif