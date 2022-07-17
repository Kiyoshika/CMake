#include "matrix.h"
#include "vector.h"
#include "util.h"

static size_t compute_offset(const size_t r, const size_t c, const size_t n_columns)
{
	return c + r * n_columns;
}

void mat_init(Matrix** mat, const size_t n_rows, const size_t n_columns)
{
	void* m_alloc = malloc(sizeof(Matrix));
	if (!m_alloc)
		util_error("Couldn't allocate memory for Matrix.");
	*mat = m_alloc;
	(*mat)->n_rows = n_rows;
	(*mat)->n_columns = n_columns;

	void* d_alloc = calloc(n_rows * n_columns, sizeof(float));
	if (!d_alloc)
		util_error("Couldn't allocate memory for Matrix data.");
	(*mat)->data = d_alloc;
}

void mat_reshape(Matrix** mat, const size_t r, const size_t c)
{
	(*mat)->n_rows = r;
	(*mat)->n_columns = c;

	void* d_alloc = calloc(r * c, sizeof(float));
	if (!d_alloc)
		util_error("Couldn't allocate enough memory when trying to reshape Matrix.");

	free((*mat)->data); // dealloc old memory before reassigning
	(*mat)->data = d_alloc;
}

Matrix* mat_create(const float* data, const size_t n_rows, const size_t n_columns)
{
	Matrix* mat = NULL;
	mat_init(&mat, n_rows, n_columns);
	size_t offset_idx = 0;

	for (size_t r = 0; r < n_rows; ++r)
	{
		for (size_t c = 0; c < n_columns; ++c)
		{
			offset_idx = compute_offset(r, c, n_columns);
			mat->data[offset_idx] = data[offset_idx];
		}
	}

	return mat;
}

void mat_random(Matrix** mat, const float lower_bound, const float upper_bound)
{
	srand(time(NULL));

	for (size_t r = 0; r < (*mat)->n_rows; ++r)
		for (size_t c = 0; c < (*mat)->n_columns; ++c)
			(*mat)->data[compute_offset(r, c, (*mat)->n_columns)] = util_rand_between(lower_bound, upper_bound);
}

void mat_fill(Matrix** mat, const float value)
{
	for (size_t i = 0; i < (*mat)->n_rows * (*mat)->n_columns; ++i)
		(*mat)->data[i] = value;
}

float mat_at(const Matrix* mat, const size_t r, const size_t c)
{
	return mat->data[compute_offset(r, c, mat->n_columns)];
}

void mat_set(Matrix** mat, const size_t r, const size_t c, const float value)
{
	(*mat)->data[compute_offset(r, c, (*mat)->n_columns)] = value;
}

Vector* mat_get_row(const Matrix* mat, const size_t row)
{
	Vector* v = NULL;
	vec_init(&v, mat->n_columns);

	for (size_t c = 0; c < mat->n_columns; ++c)
		v->data[c] = mat->data[compute_offset(row, c, mat->n_columns)];

	return v;
}

void mat_get_row_inplace(const Matrix* mat, const size_t row, Vector** vec)
{
	/*for (size_t c = 0; c < mat->n_columns; ++c)
		(*vec)->data[c] = mat->data[compute_offset(row, c, mat->n_columns)];*/

	// an optimization for copying contiguous data from a matrix
	// from experimentation, this has improved performance a decent amount
	memcpy((*vec)->data, &mat->data[row * mat->n_columns], mat->n_columns * sizeof(float));
}

Vector* mat_get_column(const Matrix* mat, const size_t column)
{
	Vector* v = NULL;
	vec_init(&v, mat->n_rows);

	for (size_t r = 0; r < mat->n_rows; ++r)
		v->data[r] = mat->data[compute_offset(r, column, mat->n_columns)];

	return v;
}

void mat_get_column_inplace(const Matrix* mat, const size_t column, Vector** vec)
{
	for (size_t r = 0; r < mat->n_rows; ++r)
		(*vec)->data[r] = mat->data[compute_offset(r, column, mat->n_columns)];
}

Matrix* mat_transpose(const Matrix* mat)
{
	Matrix* tpose = NULL;
	mat_init(&tpose, mat->n_columns, mat->n_rows);

	for (size_t r = 0; r < mat->n_rows; ++r)
		for (size_t c = 0; c < mat->n_columns; ++c)
			tpose->data[compute_offset(c, r, tpose->n_columns)] = mat->data[compute_offset(r, c, mat->n_columns)];

	return tpose;
}

void mat_transpose_inplace(const Matrix* mat, Matrix** target)
{
	// the nice thing is that we can avoid additional allocation because multiplication is commutative, so the total size of data remains the same
	(*target)->n_rows = mat->n_columns;
	(*target)->n_columns = mat->n_rows;

	// this double loop is faster than my attempt to make it linear time - probably because it had more comparisons
	for (size_t r = 0; r < mat->n_rows; ++r)
		for (size_t c = 0; c < mat->n_columns; ++c)
			(*target)->data[compute_offset(c, r, (*target)->n_columns)] = mat->data[compute_offset(r, c, mat->n_columns)];
}

void mat_print(const Matrix* mat)
{
	for (size_t r = 0; r < mat->n_rows; ++r)
	{
		for (size_t c = 0; c < mat->n_columns; ++c)
			printf("%f ", mat_at(mat, r, c));
		printf("\n");
	}
}

static Matrix* __naive_multiplication(const Matrix* mat1, const Matrix* mat2)
{
	Matrix* result = NULL;
	mat_init(&result, mat1->n_rows, mat2->n_columns);

	Vector* row_vec = NULL;
	vec_init(&row_vec, mat1->n_rows);

	Vector* col_vec = NULL;
	vec_init(&col_vec, mat2->n_columns);

	for (size_t r = 0; r < result->n_rows; ++r)
	{
		for (size_t c = 0; c < result->n_columns; ++c)
		{
			mat_get_row_inplace(mat1, r, &row_vec);
			mat_get_column_inplace(mat2, c, &col_vec);
			mat_set(&result, r, c, vec_dot(row_vec, col_vec));
		}
	}

	vec_free(&row_vec);
	vec_free(&col_vec);

	return result;
}

static Matrix* __transpose_trick(const Matrix* mat1, const Matrix* mat2)
{
	Matrix* result = NULL;
	mat_init(&result, mat1->n_rows, mat2->n_columns);

	Matrix* mat2_tpose = mat_transpose(mat2);

	// OpenMP can utilize this loop structure better than calling get_row functions which introduce too much data dependency
	for (size_t r = 0; r < result->n_rows; ++r)
	{
		for (size_t c = 0; c < result->n_columns; ++c)
		{
			for (size_t k = 0; k < mat2_tpose->n_columns; ++k)
			{
				mat_set(&result, r, c, mat_at(result, r, c) + mat_at(mat1, r, k) * mat_at(mat2_tpose, c, k));
			}
		}
	}

	mat_free(&mat2_tpose);

	return result;
}

static Matrix* __transpose_trick_parallel(const Matrix* mat1, const Matrix* mat2)
{
	Matrix* result = NULL;
	mat_init(&result, mat1->n_rows, mat2->n_columns);

	Matrix* mat2_tpose = mat_transpose(mat2);

	// OpenMP can utilize this loop structure better than calling get_row functions which introduce too much data dependency
	#pragma omp parallel for collapse(3)
	for (size_t r = 0; r < result->n_rows; ++r)
	{
		for (size_t c = 0; c < result->n_columns; ++c)
		{
			for (size_t k = 0; k < mat2_tpose->n_columns; ++k)
			{
				mat_set(&result, r, c, mat_at(result, r, c) + mat_at(mat1, r, k) * mat_at(mat2_tpose, c, k));
			}
		}
	}

	mat_free(&mat2_tpose);

	return result;
}

static void __transpose_trick_inplace(const Matrix* mat1, const Matrix* mat2, Matrix** target)
{
	// reset target to 0 incase user calls this multiple times (this caused a bug in my Perceptron...)
	mat_fill(target, 0.0f);

	Matrix* mat2_tpose = mat_transpose(mat2);

	for (size_t r = 0; r < (*target)->n_rows; ++r)
	{
		for (size_t c = 0; c < (*target)->n_columns; ++c)
		{
			for (size_t k = 0; k < mat2_tpose->n_columns; ++k)
			{
				mat_set(target, r, c, mat_at(*target, r, c) + mat_at(mat1, r, k) * mat_at(mat2_tpose, c, k));
			}
		}
	}

	mat_free(&mat2_tpose);
}

static void __transpose_trick_inplace_parallel(const Matrix* mat1, const Matrix* mat2, Matrix** target)
{
	// reset target to 0 incase user calls this multiple times (this caused a bug in my Perceptron...)
	mat_fill(target, 0.0f);

	Matrix* mat2_tpose = mat_transpose(mat2);

	Vector* row_vec = NULL;
	vec_init(&row_vec, mat1->n_rows);

	Vector* col_vec = NULL;
	vec_init(&col_vec, mat2_tpose->n_rows); // due to transpose, the size will be similar to row_vec

	#pragma omp parallel for collapse(3)
	for (size_t r = 0; r < (*target)->n_rows; ++r)
	{
		for (size_t c = 0; c < (*target)->n_columns; ++c)
		{
			for (size_t k = 0; k < mat2_tpose->n_columns; ++k)
			{
				mat_set(target, r, c, mat_at(*target, r, c) + mat_at(mat1, r, k) * mat_at(mat2_tpose, c, k));
			}
		}
	}

	mat_free(&mat2_tpose);

}

Matrix* mat_multiply(const Matrix* mat1, const Matrix* mat2)
{
	if (mat1->n_columns != mat2->n_rows)
		util_error("mat1's column size must match mat2's row size when multiplying matrices.");

	return __transpose_trick(mat1, mat2);
}

Matrix* mat_multiply_parallel(const Matrix* mat1, const Matrix* mat2)
{
	if (mat1->n_columns != mat2->n_rows)
		util_error("mat1's column size must match mat2's row size when multiplying matrices.");

	return __transpose_trick_parallel(mat1, mat2);
}

void mat_multiply_inplace(const Matrix* mat1, const Matrix* mat2, Matrix** target)
{
	if (mat1->n_columns != mat2->n_rows)
		util_error("mat1's column size must match mat2's row size when multiplying matrices.");

	__transpose_trick_inplace(mat1, mat2, target);
}

void mat_multiply_inplace_parallel(const Matrix* mat1, const Matrix* mat2, Matrix** target)
{
	if (mat1->n_columns != mat2->n_rows)
		util_error("mat1's column size must match mat2's row size when multiplying matrices.");

	__transpose_trick_inplace_parallel(mat1, mat2, target);
}

void mat_apply(Matrix** mat, float (*apply_func)(float x, float* argv), float* argv)
{
	for (size_t i = 0; i < (*mat)->n_rows * (*mat)->n_columns; ++i)
		(*mat)->data[i] = apply_func((*mat)->data[i], argv);
}

void mat_add_s(Matrix** mat, const float value)
{
	for (size_t i = 0; i < (*mat)->n_columns * (*mat)->n_rows; ++i)
		(*mat)->data[i] += value;
}

void mat_substract_s(Matrix** mat, const float value)
{
	for (size_t i = 0; i < (*mat)->n_columns * (*mat)->n_rows; ++i)
		(*mat)->data[i] -= value;
}

void mat_multiply_s(Matrix** mat, const float value)
{
	for (size_t i = 0; i < (*mat)->n_columns * (*mat)->n_rows; ++i)
		(*mat)->data[i] *= value;
}

void mat_divide_s(Matrix** mat, const float value)
{
	for (size_t i = 0; i < (*mat)->n_columns * (*mat)->n_rows; ++i)
		(*mat)->data[i] /= value;
}

void mat_add_e(Matrix** target, const Matrix* mat)
{
	if (mat->n_rows != (*target)->n_rows && mat->n_columns != (*target)->n_columns)
		util_error("Matrix dimensions must match exactly when trying to perform element-wise operations.");

	for (size_t i = 0; i < mat->n_rows * mat->n_columns; ++i)
		(*target)->data[i] += mat->data[i];
}

void mat_subtract_e(Matrix** target, const Matrix* mat)
{
	if (mat->n_rows != (*target)->n_rows && mat->n_columns != (*target)->n_columns)
		util_error("Matrix dimensions must match exactly when trying to perform element-wise operations.");

	for (size_t i = 0; i < mat->n_rows * mat->n_columns; ++i)
		(*target)->data[i] -= mat->data[i];
}

void mat_multiply_e(Matrix** target, const Matrix* mat)
{
	if (mat->n_rows != (*target)->n_rows && mat->n_columns != (*target)->n_columns)
		util_error("Matrix dimensions must match exactly when trying to perform element-wise operations.");

	for (size_t i = 0; i < mat->n_rows * mat->n_columns; ++i)
		(*target)->data[i] *= mat->data[i];
}

void mat_divide_e(Matrix** target, const Matrix* mat)
{
	if (mat->n_rows != (*target)->n_rows && mat->n_columns != (*target)->n_columns)
		util_error("Matrix dimensions must match exactly when trying to perform element-wise operations.");

	for (size_t i = 0; i < mat->n_rows * mat->n_columns; ++i)
		(*target)->data[i] /= mat->data[i];
}

Matrix* mat_copy(const Matrix* mat)
{
	Matrix* mcpy = NULL;
	mat_init(&mcpy, mat->n_rows, mat->n_columns);

	for (size_t i = 0; i < mat->n_rows * mat->n_columns; ++i)
		mcpy->data[i] = mat->data[i];

	return mcpy;
}

Matrix* mat_subset(const Matrix* mat, const size_t r_lower, const size_t r_upper, const size_t c_lower, const size_t c_upper)
{
	Matrix* subset = NULL;
	mat_init(&subset, r_upper - r_lower + 1, c_upper - c_lower + 1);

	for (size_t r = r_lower; r <= r_upper; ++r)
		for (size_t c = c_lower; c <= c_upper; ++c)
			mat_set(&subset, r - r_lower, c - c_lower, mat_at(mat, r, c));
	
	return subset;
}

float mat_sum(const Matrix* mat)
{
	float result = 0.0f;
	for (size_t i = 0; i < mat->n_rows * mat->n_columns; ++i)
		result += mat->data[i];

	return result;
}

float mat_mean(const Matrix* mat)
{
	return mat_sum(mat) / (float)(mat->n_rows * mat->n_columns);
}

static bool check_used_index(size_t* used_indices, size_t n_indices, size_t search_index)
{
	for (size_t i = 0; i < n_indices; ++i)
		if (used_indices[i] == search_index)
			return true;

	return false;
}

Matrix* mat_sample(const Matrix* mat, const size_t n_samples, bool with_replacement, size_t* sample_indices)
{
	if (!with_replacement && n_samples > mat->n_rows)
		util_error("Cannot sample without replacement with a size larger than the matrix's row count.");

	Matrix* sample = NULL;
	mat_init(&sample, n_samples, mat->n_columns);

	// would be better to insert the elements sorted and binary search, but for now
	// I will use a naive linear implementation
	size_t* used_indices = calloc(n_samples, sizeof(size_t));
	size_t n_used_indices = 0;

	srand(time(NULL));

	while (n_used_indices < n_samples)
	{
		size_t rand_idx = (size_t)util_rand_between(0.0f, (float)mat->n_rows);
		if (n_used_indices > 0 && !with_replacement && !check_used_index(used_indices, n_used_indices, rand_idx))
		{
			for (size_t c = 0; c < sample->n_columns; ++c)
				mat_set(&sample, n_used_indices, c, mat_at(mat, rand_idx, c));

			used_indices[n_used_indices] = rand_idx;

			if (sample_indices)
				sample_indices[n_used_indices] = rand_idx;

			n_used_indices++;
		}
		// I break out this condition separately as used_indices is filled with zeroes, 
		// so check_used_index would always return true on 0, so technically it's possible
		// that we would never sample the very first row
		else if (n_used_indices == 0 && !with_replacement)
		{
			for (size_t c = 0; c < sample->n_columns; ++c)
				mat_set(&sample, n_used_indices, c, mat_at(mat, rand_idx, c));

			used_indices[n_used_indices] = rand_idx;

			if (sample_indices)
				sample_indices[n_used_indices] = rand_idx;

			n_used_indices++;

		}
		else if (with_replacement)
		{
			for (size_t c = 0; c < sample->n_columns; ++c)
				mat_set(&sample, n_used_indices, c, mat_at(mat, rand_idx, c));

			used_indices[n_used_indices] = rand_idx;

			if (sample_indices)
				sample_indices[n_used_indices] = rand_idx;

			n_used_indices++;
		}
	}

	free(used_indices);

	return sample;
}

void mat_free(Matrix** mat)
{
	free((*mat)->data);
	(*mat)->data = NULL;

	free(*mat);
	*mat = NULL;
}
