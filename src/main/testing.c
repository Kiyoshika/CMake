#include <stdio.h>
#include "matrix.h"

int main()
{
	Matrix* mat = NULL;
	mat_init(&mat, 10, 3);
	mat_random(&mat, 1.0f, 5.0f);

	printf("Before Sorting:\n");
	mat_print(mat);

	mat_sort(&mat, 1, false);

	printf("\n\nAfter Sorting:\n");
	mat_print(mat);	

	mat_free(&mat);

	return 0;
}
