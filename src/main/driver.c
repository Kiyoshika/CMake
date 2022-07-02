#include "vector.h"
#include "matrix.h"

int main()
{
	// mat_multiply - ~430MB & 31 sec on x86 Release mode

	Matrix* mat = NULL;
	mat_init(&mat, 10000, 25);
	mat_random(&mat, 1.0, 5.0);

	printf("Mean: %f\n", mat_mean(mat));

	mat_free(&mat);

	return 0;
}