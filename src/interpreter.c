#include "interpreter.h"
#include "ast.h"
#include "sv.h"
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

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
		assert(0 && "Proc call not implemented");
	}
	if (exp->type == EXPRESSION_TYPE_EEO) {
		ictx_process_expression(ictx, exp->EEO.left);
		ictx_process_expression(ictx, exp->EEO.right);
		// perform operation on stack
		switch (exp->EEO.operation.type) {
			case OPERATOR_TYPE_ARITH:
				{
					// the stack SHOULD have the two values to add
					stack_node l = ictx->stack[ictx->stack_top--];
					stack_node r = ictx->stack[ictx->stack_top--];
					switch (exp->EEO.operation.op) {
						case '+':
							{
								// DBL + DBL = DBL
								// DBL + INT = DBL
								// INT + DBL = DBL
								if (l.type == DOUBLE || r.type == DOUBLE) {
									ictx->stack[++ictx->stack_top].type = DOUBLE;
									// INT + DBL
									if (l.type == INTEGER) {
										ictx->stack[ictx->stack_top].doubleLiteral = l.integerLiteral + r.doubleLiteral;
									}
									// DBL + INT
									else if (r.type == INTEGER) {
										ictx->stack[ictx->stack_top].doubleLiteral = l.doubleLiteral + r.integerLiteral;
									}
									// DBL + DBL
									else {
										ictx->stack[ictx->stack_top].doubleLiteral = l.doubleLiteral + r.doubleLiteral;
									}
									break;
								}
								// INT + INT = INT
								if (l.type == INTEGER && r.type == INTEGER) {
									ictx->stack[++ictx->stack_top].type = INTEGER;
									ictx->stack[ictx->stack_top].integerLiteral = l.integerLiteral + r.integerLiteral;
									break;
								}
							}
					}
				} break;
			default: assert(0 && "StackOP, and LogicOp not implemented");
				// ictx->stack[ictx->stack_top].type = 
		}
	}
}

void ictx_run(interpreter_ctx* ictx, Program p) {
	for (AST_Node* n = cvector_begin(p.p); n != cvector_end(p.p); n++) {
		// ictx_show_stack(ictx);
		if (n->nodeType == AST_NODE_TYPE_EXPRESSION) {
			ictx_process_expression(ictx, n->expression);
		}
	}
}
