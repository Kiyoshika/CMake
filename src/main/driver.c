#include <stdio.h>
#include "matrix.h"

int main()
{
	Matrix* mat = NULL;
	mat_init(&mat, 4096, 4096);
	
	Matrix* mat2 = mat_copy(mat);
	Matrix* prod = mat_multiply_parallel(mat, mat2);

	mat_free(&mat);
	mat_free(&mat2);
	mat_free(&prod);

	return 0;
}
