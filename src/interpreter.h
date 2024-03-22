#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "sv.h"

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

int interpret_decimal_sv_to_int(String_View);
int interpret_hex_sv_to_int(String_View);
double interpret_double_sv_to_double(String_View);

#endif
