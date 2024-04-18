#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "sv.h"
#include "ast.h"
#include "tokenizer.h"

#define STACK_SIZE 500

typedef enum {
	UNDEFINED=0, CHAR, STRING, DOUBLE, INTEGER, STACK_OP
} stack_node_type;

typedef struct {
	stack_node_type type;
	tokenizer_state state;
	union {
		String_View stringLiteral; // covers char and string 
		double doubleLiteral;
		int integerLiteral;
		String_View charLiteral;
		StackOp stackOp;
	};
} stack_node;

typedef struct {
	// Data stack
	stack_node stack[STACK_SIZE];
	int        stack_top;

	stack_node peeked;
} interpreter_ctx;


const char* ictx_stack_node_type_to_str(stack_node_type);

interpreter_ctx ictx_new();
void  					ictx_run(interpreter_ctx*, Program);

// actions
void ictx_process_stmt_expr(interpreter_ctx*, StatementExpression);
void ictx_process_expression(interpreter_ctx*, Expression*);
void ictx_process_statement(interpreter_ctx*, Statement*);
void ictx_process_iff(interpreter_ctx*, Iff);
void ictx_process_block(interpreter_ctx*, Block);
#endif
