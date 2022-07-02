#include "vector.h"
#include "matrix.h"

int main()
{
	Matrix* mat = NULL;
	mat_init(&mat, 20, 20);
	mat_random(&mat, 1.0, 5.0);

	Matrix* mat2 = NULL;
	mat_init(&mat2, 20, 20);
	mat_random(&mat2, 1.0, 5.0);

	for (size_t i = 0; i < 5000; ++i)
	{
		Matrix* product = mat_multiply(mat, mat2);
		mat_free(&product);
	}

	mat_free(&mat);
	mat_free(&mat2);
	return 0;
}