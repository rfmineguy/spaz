#ifndef SL_ASSERT_H
#define SL_ASSERT_H

#define sl_assert(expr, fmt, ...) \
	if ((expr)) {\
		fprintf(stderr, fmt, ##__VA_ARGS__);\
		exit(90);\
	}

#endif
