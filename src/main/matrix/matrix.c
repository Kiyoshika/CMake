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
	for (size_t c = 0; c < mat->n_columns; ++c)
		(*vec)->data[c] = mat->data[compute_offset(row, c, mat->n_columns)];
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

Matrix* mat_multiply(const Matrix* mat1, const Matrix* mat2)
{
	if (mat1->n_columns != mat2->n_rows)
		util_error("mat1's column size must match mat2's row size when multiplying matrices.");

	// (n, k) * (k, t) = (n, t)
	Matrix* result = NULL;
	mat_init(&result, mat1->n_rows, mat2->n_columns);

	Vector* row_vec = NULL;
	vec_init(&row_vec, mat1->n_columns);

	Vector* column_vec = NULL;
	vec_init(&column_vec, mat2->n_rows);

	for (size_t r = 0; r < mat1->n_rows; ++r)
	{
		for (size_t c = 0; c < mat2->n_columns; ++c)
		{
			mat_get_row_inplace(mat1, r, &row_vec);
			mat_get_column_inplace(mat2, c, &column_vec);
			result->data[compute_offset(r, c, result->n_columns)] = vec_dot(row_vec, column_vec);
		}
	}

	vec_free(&row_vec);
	vec_free(&column_vec);

	return result;
}

Matrix* mat_multiply_exp(const Matrix* mat1, const Matrix* mat2)
{
	if (mat1->n_columns != mat2->n_rows)
		util_error("mat1's column size must match mat2's row size when multiplying matrices.");

	// (n, k) * (k, t) = (n, t)
	Matrix* result = NULL;
	mat_init(&result, mat1->n_rows, mat2->n_columns);

	Vector* row_vec = NULL;
	vec_init(&row_vec, mat1->n_columns);

	Vector* column_vec = NULL;
	vec_init(&column_vec, mat2->n_rows);

	for (size_t r = 0; r < mat1->n_columns; ++r)
	{
		for (size_t c = 0; c < mat2->n_rows; ++c)
		{
			mat_get_row_inplace(mat1, r, &row_vec);
			mat_get_column_inplace(mat2, c, &column_vec);
			result->data[compute_offset(r, c, result->n_columns)] = vec_dot(row_vec, column_vec);
		}
	}

	vec_free(&row_vec);
	vec_free(&column_vec);

	return result;
}

void mat_multiply_inplace(const Matrix* mat1, const Matrix* mat2, Matrix** target)
{
	if (mat1->n_columns != mat2->n_rows)
		util_error("mat1's column size must match mat2's row size when multiplying matrices.");

	Vector* row_vec = NULL;
	vec_init(&row_vec, mat1->n_columns);

	Vector* column_vec = NULL;
	vec_init(&column_vec, mat2->n_rows);

	for (size_t r = 0; r < mat1->n_rows; ++r)
	{
		for (size_t c = 0; c < mat2->n_columns; ++c)
		{
			mat_get_row_inplace(mat1, r, &row_vec);
			mat_get_column_inplace(mat2, c, &column_vec);
			(*target)->data[compute_offset(r, c, (*target)->n_columns)] = vec_dot(row_vec, column_vec);
		}
	}

	vec_free(&row_vec);
	vec_free(&column_vec);
}

void mat_apply(Matrix** mat, float (*apply_func)(float x, float* argv), float* argv)
{
	for (size_t i = 0; i < (*mat)->n_rows * (*mat)->n_columns; ++i)
		(*mat)->data[i] = apply_func((*mat)->data[i], argv);
}

void mat_free(Matrix** mat)
{
	free((*mat)->data);
	(*mat)->data = NULL;

	free(*mat);
	*mat = NULL;
}