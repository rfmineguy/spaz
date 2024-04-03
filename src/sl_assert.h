#ifndef SL_ASSERT_H
#define SL_ASSERT_H
#include "b_stacktrace.h"

#define sl_assert(expr, fmt, ...) \
	if (!(expr)) {\
		char* s = b_stacktrace_get_string();\
		fprintf(stderr, "%s\n", s);\
		fprintf(stderr, "(%d) " fmt, __LINE__, ##__VA_ARGS__);\
		free(s);\
		exit(90);\
	}

#endif
