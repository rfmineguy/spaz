#include "interpreter.h"
#include "ast.h"
#include "sv.h"
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "sl_assert.h"

/***
 *  For interpreting the code, I think building a parse tree is worth while
 *  Making use of a symbol table in an interesting way could be handy for implementing function calls later on as well
 */

interpreter_ctx ictx_new() {
	interpreter_ctx ctx = {0};
	ctx.stack_top = -1;
	return ctx;
}

void ictx_show_stack(interpreter_ctx* ictx) {
	for (int i = 0; i <= ictx->stack_top; i++) {
		switch (ictx->stack[i].type) {
			case DOUBLE:
				printf("%04.f\n", ictx->stack[i].doubleLiteral);
				break;
			case INTEGER:
				printf("%d\n", ictx->stack[i].integerLiteral);
				break;
			case CHAR:
				printf(SV_Fmt "\n", SV_Arg(ictx->stack[i].charLiteral));
				break;
			case STRING:
				printf(SV_Fmt "\n", SV_Arg(ictx->stack[i].stringLiteral));
				break;
		}
	}
}

void ictx_process_expression(interpreter_ctx* ictx, Expression* exp) {
	if (exp->type == EXPRESSION_TYPE_TERM) {
		switch (exp->ETerm.term.type) {
			case TERM_TYPE_CHR_LIT:
				ictx->stack_top++;
				ictx->stack[ictx->stack_top].type = CHAR;
				ictx->stack[ictx->stack_top].charLiteral = exp->ETerm.term._chr;
				break;
			case TERM_TYPE_STRING_LIT:
				ictx->stack_top++;
				ictx->stack[ictx->stack_top].type = STRING;
				ictx->stack[ictx->stack_top].stringLiteral = exp->ETerm.term._string;
				break;
			case TERM_TYPE_HEX_LIT:
			case TERM_TYPE_DEC_LIT:
				ictx->stack_top++;
				ictx->stack[ictx->stack_top].type = INTEGER;
				ictx->stack[ictx->stack_top].integerLiteral = exp->ETerm.term._integer;
				break;
			case TERM_TYPE_DOUBLE_LIT:
				ictx->stack_top++;
				ictx->stack[ictx->stack_top].type = DOUBLE;
				ictx->stack[ictx->stack_top].doubleLiteral = exp->ETerm.term._double;
				break;
		}
		return;
	}
	if (exp->type == EXPRESSION_TYPE_STACK_OP) {
		assert(0 && "Stack op not implemented");
	}
	if (exp->type == EXPRESSION_TYPE_PROC_CALL) {
		if (sv_eq(exp->EProcCall.proc_call.name, SV("print"))) {
			stack_node l = ictx->stack[ictx->stack_top];
			/// ictx_show_stack(ictx);
			switch (l.type) {
				case INTEGER:
					printf("%d\n", l.integerLiteral);
					break;
				case CHAR:
					printf(SV_Fmt "\n", SV_Arg(l.charLiteral));
					break;
				case STRING:
					printf(SV_Fmt "\n", SV_Arg(l.stringLiteral));
					break;
				case DOUBLE:
					printf("%0.4f\n", l.doubleLiteral);
					break;
			}
			return;
		}
		sl_assert(0, "Proc call not implemented");
	}
	if (exp->type == EXPRESSION_TYPE_EEO) {
		ictx_process_expression(ictx, exp->EEO.left);
		ictx_process_expression(ictx, exp->EEO.right);
		// the stack SHOULD have the two values to add
		stack_node r = ictx->stack[ictx->stack_top--];
		stack_node l = ictx->stack[ictx->stack_top--];
		switch (exp->EEO.operation.op) {
			case '+':
				{
					stack_node n = ictx_perform_addition(l, r);
					ictx->stack[++ictx->stack_top] = n;
					break;
				}
			case '-':
				{
					stack_node n = ictx_perform_subtraction(l, r);
					ictx->stack[++ictx->stack_top] = n;
					break;
				}
			case '*':
				{
					stack_node n = ictx_perform_mult(l, r);
					ictx->stack[++ictx->stack_top] = n;
					break;
				}
			case '/':
				{
					stack_node n = ictx_perform_division(l, r);
					ictx->stack[++ictx->stack_top] = n;
					break;
				}
			default:
				sl_assert(0, "Operator '%c' not implmented", exp->EEO.operation.op);
		}
	}
}

#define ARITH_OPERATION(l, r, resulttype, ltype, rtype, expr) \
	if (l.type == ltype && r.type == rtype) {\
		n.type = resulttype;\
		(expr);\
		return n;\
	}

stack_node ictx_perform_addition(stack_node l, stack_node r) {
	stack_node n;
	ARITH_OPERATION(l, r, DOUBLE,  DOUBLE , DOUBLE,  n.doubleLiteral  = l.doubleLiteral  + r.doubleLiteral);
	ARITH_OPERATION(l, r, INTEGER, DOUBLE , DOUBLE,  n.doubleLiteral  = l.integerLiteral + r.doubleLiteral);
	ARITH_OPERATION(l, r, DOUBLE,  INTEGER, DOUBLE,  n.doubleLiteral  = l.doubleLiteral  + r.integerLiteral);
	ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral + r.integerLiteral);
	assert(0 && "This type of addition is not supported");
}

stack_node ictx_perform_subtraction(stack_node l, stack_node r) {
	stack_node n;
	ARITH_OPERATION(l, r, DOUBLE,  DOUBLE , DOUBLE,  n.doubleLiteral  = l.doubleLiteral  - r.doubleLiteral);
	ARITH_OPERATION(l, r, INTEGER, DOUBLE , DOUBLE,  n.doubleLiteral  = l.integerLiteral - r.doubleLiteral);
	ARITH_OPERATION(l, r, DOUBLE,  INTEGER, DOUBLE,  n.doubleLiteral  = l.doubleLiteral  - r.integerLiteral);
	ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral - r.integerLiteral);
	assert(0 && "This type of addition is not supported");
}

stack_node ictx_perform_mult(stack_node l, stack_node r) {
	stack_node n;
	ARITH_OPERATION(l, r, DOUBLE,  DOUBLE , DOUBLE,  n.doubleLiteral  = l.doubleLiteral  * r.doubleLiteral);
	ARITH_OPERATION(l, r, INTEGER, DOUBLE , DOUBLE,  n.doubleLiteral  = l.integerLiteral * r.doubleLiteral);
	ARITH_OPERATION(l, r, DOUBLE,  INTEGER, DOUBLE,  n.doubleLiteral  = l.doubleLiteral  * r.integerLiteral);
	ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral * r.integerLiteral);
	assert(0 && "This type of addition is not supported");
}

stack_node ictx_perform_division(stack_node l, stack_node r) {
	stack_node n;
	ARITH_OPERATION(l, r, DOUBLE,  DOUBLE , DOUBLE,  n.doubleLiteral  = l.doubleLiteral  / r.doubleLiteral);
	ARITH_OPERATION(l, r, INTEGER, DOUBLE , DOUBLE,  n.doubleLiteral  = l.integerLiteral / r.doubleLiteral);
	ARITH_OPERATION(l, r, DOUBLE,  INTEGER, DOUBLE,  n.doubleLiteral  = l.doubleLiteral  / r.integerLiteral);
	ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral / r.integerLiteral);
	assert(0 && "This type of addition is not supported");
}
void ictx_run(interpreter_ctx* ictx, Program p) {
	for (AST_Node* n = cvector_begin(p.p); n != cvector_end(p.p); n++) {
		// ictx_show_stack(ictx);
		if (n->nodeType == AST_NODE_TYPE_EXPRESSION) {
			ictx_process_expression(ictx, n->expression);
		}
	}
}
