#include "util.h"

void util_error(const char* msg)
{
	printf("%s\n", msg);
	exit(-1);
}

float util_rand_between(const float lower_bound, const float upper_bound)
{
	return lower_bound + ((float)rand() / (float)RAND_MAX) * (upper_bound - lower_bound);
}