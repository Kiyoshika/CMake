#include "vector.h"
#include "util.h"

void vec_init(Vector** vec, const size_t n_elem)
{
	void* v_alloc = malloc(sizeof(Vector));
	if (!v_alloc)
		util_error("Couldn't allocate memory for vector.");
	*vec = v_alloc;
	(*vec)->n_elem = n_elem;

	void* d_alloc = calloc(n_elem, sizeof(float));
	if (!d_alloc)
		util_error("Couldn't allocate memory for vector data.");
	(*vec)->data = d_alloc;
}

Vector* vec_create(const float* data, const size_t n_elem)
{
	Vector* vec = NULL;
	vec_init(&vec, n_elem);

	for (size_t i = 0; i < n_elem; ++i)
		vec->data[i] = data[i];

	return vec;
}

Vector* vec_copy(const Vector* vec)
{
	Vector* v_copy = NULL;
	vec_init(&v_copy, vec->n_elem);
	for (size_t i = 0; i < vec->n_elem; ++i)
		v_copy->data[i] = vec->data[i];

	return v_copy;
}

float vec_at(const Vector* vec, const size_t i)
{
	return vec->data[i];
}

void vec_set(Vector** vec, const size_t i, const float value)
{
	(*vec)->data[i] = value;
}

float vec_dot(const Vector* vec1, const Vector* vec2)
{
	if (vec1->n_elem != vec2->n_elem)
		util_error("Vectors must be same size before taking dot product.");

	float result = 0.0f;
	for (size_t i = 0; i < vec1->n_elem; ++i)
		result += vec1->data[i] * vec2->data[i];

	return result;
}

void vec_random(Vector** vec, const float lower_bound, const float upper_bound)
{
	srand(time(NULL));

	for (size_t i = 0; i < (*vec)->n_elem; ++i)
		(*vec)->data[i] = util_rand_between(lower_bound, upper_bound);
}

void vec_fill(Vector** vec, const float value)
{
	for (size_t i = 0; i < (*vec)->n_elem; ++i)
		(*vec)->data[i] = value;
}

void vec_apply(Vector** vec, float (*apply_func)(float x, float* argv), float* argv)
{
	for (size_t i = 0; i < (*vec)->n_elem; ++i)
		(*vec)->data[i] = apply_func((*vec)->data[i], argv);
}

void vec_add_s(Vector** vec, const float value)
{
	for (size_t i = 0; i < (*vec)->n_elem; ++i)
		(*vec)->data[i] += value;
}

void vec_subtract_s(Vector** vec, const float value)
{
	for (size_t i = 0; i < (*vec)->n_elem; ++i)
		(*vec)->data[i] -= value;
}

void vec_multiply_s(Vector** vec, const float value)
{
	for (size_t i = 0; i < (*vec)->n_elem; ++i)
		(*vec)->data[i] *= value;
}

void vec_divide_s(Vector** vec, const float value)
{
	for (size_t i = 0; i < (*vec)->n_elem; ++i)
		(*vec)->data[i] /= value;
}

void vec_add_e(Vector** target, const Vector* vec)
{
	for (size_t i = 0; i < (*target)->n_elem; ++i)
		(*target)->data[i] += vec->data[i];
}

void vec_subtract_e(Vector** target, const Vector* vec)
{
	for (size_t i = 0; i < (*target)->n_elem; ++i)
		(*target)->data[i] -= vec->data[i];
}

void vec_multiply_e(Vector** target, const Vector* vec)
{
	for (size_t i = 0; i < (*target)->n_elem; ++i)
		(*target)->data[i] *= vec->data[i];
}

void vec_divide_e(Vector** target, const Vector* vec)
{
	for (size_t i = 0; i < (*target)->n_elem; ++i)
		(*target)->data[i] /= vec->data[i];
}

float vec_sum(const Vector* vec)
{
	float result = 0.0f;
	for (size_t i = 0; i < vec->n_elem; ++i)
		result += vec->data[i];
	
	return result;
}

float vec_mean(const Vector* vec)
{
	return vec_sum(vec) / (float)vec->n_elem;
}

void vec_free(Vector** vec)
{
	free((*vec)->data);
	(*vec)->data = NULL;

	free(*vec);
	*vec = NULL;
}