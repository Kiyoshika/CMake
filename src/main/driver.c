#include "vector.h"
#include "matrix.h"


int main()
{
	Matrix* mat = NULL;
	mat_init(&mat, 10000, 10);
	mat_random(&mat, 1.0, 5.0);

	Matrix* mat2 = NULL;
	mat_init(&mat2, 10, 10000);
	mat_random(&mat2, 1.0, 5.0);

	Matrix* product = mat_multiply(mat, mat2);

	mat_free(&mat);
	mat_free(&mat2);
	mat_free(&product);

	return 0;
}