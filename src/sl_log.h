#ifndef SL_LOG_H
#define SL_LOG_H
#include <stdio.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define sl_log(fmt, ...) \
	fprintf(stdout, "(%s, %3d): " fmt "\n", (__FILENAME__), __LINE__, ##__VA_ARGS__)


/**
 *  Macro for logging the memory freeing progress
 */
#if defined(DEBUG_FREE) && DEBUG_FREE==1
#define sl_log_free(fmt, ...) sl_log("(FREE)" fmt, ##__VA_ARGS__)
#else
#define sl_log_free(fmt, ...) {}
#endif

/**
 *  Macro for logging the generated parse tree
 */
#define sl_log_ast(fmt, ...)  sl_log("(AST) " fmt, ##__VA_ARGS__)

#endif // SL_LOG_H
