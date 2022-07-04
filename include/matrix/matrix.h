#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct Matrix
{
	float* data;
	size_t n_rows;
	size_t n_columns;
} Matrix;

// forward declaration
typedef struct Vector Vector;

// initialize matrix with n_rows and n_columns
void mat_init(Matrix** mat, const size_t n_rows, const size_t n_columns);

// reshape matrix to new dimensions (does not touch original data, i.e., does not reset everything to 0)
void mat_reshape(Matrix** mat, const size_t r, const size_t c);

// create matrix from C-style 2D array (must cast to float pointer)
Matrix* mat_create(const float* data, const size_t n_rows, const size_t n_colums);

// generate random values for matrix
void mat_random(Matrix** mat, const float lower_bound, const float upper_bound);

// fill matrix with a value
void mat_fill(Matrix** mat, const float value);

// return matrix value at index (r, c)
float mat_at(const Matrix* mat, const size_t r, const size_t c);

// set matrix value at index (r, c)
void mat_set(Matrix** mat, const size_t r, const size_t c, const float value);

// create vector from specific row index of matrix - don't forget to free this returned vector
Vector* mat_get_row(const Matrix* mat, const size_t row);

// create vector from specific row index of matrix and store it into target (assumes it's pre-allocated)
void mat_get_row_inplace(const Matrix* mat, const size_t row, Vector** vec);

// create vector from specific column index of matrix - don't forget to free this returned vector
Vector* mat_get_column(const Matrix* mat, const size_t column);

// create vector from specific column index of matrix and store it into target (assumes it's pre-allocated)
void mat_get_column_inplace(const Matrix* mat, const size_t column, Vector** vec);

// transpose matrix and return it as a new matrix - careful, be sure to free your matrix before reassigning if it is non-null
Matrix* mat_transpose(const Matrix* mat);

// transpose matrix and store it into target (assumes it's pre-allocated). you can use this version if you are worried about heap fragmentation (i.e., if you are doing an absurd amount of transposes). NOTE: this will automatically swap row & column incides for you if you call transpose inplace multiple times.
void mat_transpose_inplace(const Matrix* mat, Matrix** target);

// multiply two matrices and return a new matrix with the result - careful, be sure to free your matrix before reassigning if it is non-null
Matrix* mat_multiply(const Matrix* mat1, const Matrix* mat2);

// multiply two matrices using OpenMP for multiple threads. new matrix is returned - careful, be sure to free your matrix before reassigning if it is non-null
Matrix* mat_multiply_parallel(const Matrix* mat1, const Matrix* mat2);

// multiply two matrices and store the result into target (assumes it's pre-allocated). you can use this version if you are worried about heap fragmentation (i.e., if you are doing an absurd amount of multiplications).
void mat_multiply_inplace(const Matrix* mat1, const Matrix* mat2, Matrix** target);

// apply function to each element in matrix inplace using function pointer. function pointer uses argv if user needs to pass any additional parameters to the apply function, otherwise can pass NULL. value returned from function will be set in the matrix's cell.
void mat_apply(Matrix** mat, float (*apply_func)(float x, float* argv), float* argv);

// print all cells of matrix to standard output
void mat_print(const Matrix* mat);

// add scalar to each element inplace
void mat_add_s(Matrix** mat, const float value);

// subtract scalar from each element inplace
void mat_subtract_s(Matrix** mat, const float value);

// multiply scalar to each element inplace
void mat_multiply_s(Matrix** mat, const float value);

// divide scalar to each element inplace
void mat_divide_s(Matrix** mat, const float value);

// add matrices target + mat element-wise - target will be modified inplace. NOTE: dimensions must be exact
void mat_add_e(Matrix** target, const Matrix* mat);

// subtract matrices target - mat element-wise - target will be modified inplace. NOTE: dimensions must be exact
void mat_subtract_e(Matrix** target, const Matrix* mat);

// multiply matrices target * mat element-wise - target will be modified inplace. NOTE: dimensions must be exact
void mat_multiply_e(Matrix** target, const Matrix* mat);

// divide matrices target / mat element-wise - target will be modified inplace. NOTE: dimensions must be exact
void mat_divide_e(Matrix** target, const Matrix* mat);

// copy contents from mat into target
Matrix* mat_copy(const Matrix* mat);

// select a subset of a matrix to return. rows start from r_lower to r_upper and columns start from c_lower to c_upper.
Matrix* mat_subset(const Matrix* mat, const size_t r_lower, const size_t r_upper, const size_t c_lower, const size_t c_upper);

// sum all elements in matrix
float mat_sum(const Matrix* mat);

// compute mean for all elements in matrix
float mat_mean(const Matrix* mat);

// randomly sample rows with or without replacement. optionally store the incides sampled into sampled_indices (e.g., for paired sampling) - sampled_indices must be of length n_samples and is assumed to be pre-allocated. user can pass NULL if they don't need the sampled indices.
Matrix* mat_sample(const Matrix* mat, const size_t n_samples, bool with_replacement, size_t* sampled_indices);

// free memory allocated by matrix
void mat_free(Matrix** mat);

#endif
