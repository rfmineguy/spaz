#ifndef AST_H
#define AST_H

/**
 *    ---------------- TERMINALS ----------------------
 *    id     				 := [a-zA-Z][a-zA-Z0-9_]*
 * 		hexlit         := 0x[0-9a-fA-F]+
 *    dbllit         := [0-9]+\\.[0-9]+
 * 		declit         := [0-9]+
 *    strlit         := \\"([^\\"]|\n)*\\"
 * 		stack_op       := ',' | '.'
 * 		arith_op       := '+' | '-' | '*' | '/' | '%'
 * 		logic_op       := "&&" | "||" | '>' | '<' | "==" | ">=" | "<="
 *    -------------- NON-TERMINALS ----------------------
 * 		term           := <declit> | <hexlit> | <dbllit> | <strlit>
 * 		operator       := <arith_op> | <logic_op>
 *    expression     := <expression> <expression> <operator> | <term> | <stack_op>
 *    procedure_call := <expression> <id>
 *    if   					 := if <expression> <block>
 *    switch				 := switch <stack_op> <switch-block>
 *    procedure_call := ...
 * 		statement  		 := <if> | <switch> | <procedure_call> | <statement> <stack_op>
 * 		statements     := <statement> | <statement> <statements>
 * 		plist          := <id> | <id> <plist>
 * 		block          := '{' <statements> '}'
 * 		switch-block   := ...
 * 		procedure_def  := <id> <plist> <block>
 */

#include "cvector.h"
#include "sv.h"
typedef enum TerminalType {
	
} TerminalType;
typedef enum TermType {
	TT_INTEGER, TT_DOUBLE, TT_STRING, TT_IDENT 
} TermType;
typedef enum ExpressionType {
	ET_TERM, ET_STACK_OP, ET_COMPOUND 
} ExpressionType;

typedef struct Terminal Terminal;
typedef struct Term Term;
typedef struct Expression Expression;
typedef struct ProcedureDef ProcedureDef;
typedef struct Statement Statement;
typedef struct ProcedureCall ProcedureCall;
typedef struct Iff Iff;
typedef struct Switch Switch;
typedef struct Case Case;
typedef struct Block Block;

struct Terminal {
};

struct Term {
	TermType type;
	union {
		int         _integer;
		double      _double;
		String_View _string;
		String_View _ident;
	};
};

/** 
 *   when type == TERM
 *     - only term is guarunteed to be a valid value
 *   when type == STACK_OP
 *     - only operation is guarunteed to be valid
 *   when type == COMPOUND
 *     - only term, term2, and operation are guarunteed to be valid
 */
struct Expression {
	ExpressionType type;
	Term term, term2;
	char operation;
};

struct ProcedureCall {
	String_View name;
	int argumentCount;
};

struct Block {
};

struct ProcedureDef {
	String_View name;
	cvector_vector_type(String_View) params;
	Block block;
};

struct Iff {
	Expression expression;
	Block block;
};

struct Switch {
};

struct Case {
};

#endif
