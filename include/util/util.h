#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// print error message and terminate application
void util_error(const char* msg);

// generate random float between lower_bound and upper_bound. NOTE: need to provide seed before calling this function!
float util_rand_between(const float lower_bound, const float upper_bound);

#endif