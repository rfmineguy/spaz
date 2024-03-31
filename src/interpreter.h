#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "sv.h"
#include "ast.h"

#define STACK_SIZE 500

typedef enum {
	STRING, DOUBLE, INTEGER
} stack_node_type;

typedef struct {
	stack_node_type type;
	union {
		String_View stringLiteral; // covers char and string 
		double doubleLiteral;
		int integerLiteral;
	};
} stack_node;

typedef struct {
	// Data stack
	stack_node stack[STACK_SIZE];
	int        stack_top;
} interpreter_ctx;

interpreter_ctx ictx_new();
void  					ictx_run(interpreter_ctx*, Program);
#endif
