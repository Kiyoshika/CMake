#include "vector.h"
#include "matrix.h"


int main()
{
	// mat_multiply - ~430MB & 31 sec on x86 Release mode

	Matrix* mat = NULL;
	mat_init(&mat, 4096, 4096);
	mat_random(&mat, 1.0, 5.0);

	Matrix* mat2 = NULL;
	mat_init(&mat2, 4096, 4096);
	mat_random(&mat2, 1.0, 5.0);

	Matrix* product = mat_multiply_parallel(mat, mat2);

	mat_free(&mat);
	mat_free(&mat2);
	mat_free(&product);

	return 0;
}