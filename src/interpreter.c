#include "interpreter.h"
#include "interpreter_builtins.h"
#include "ast.h"
#include "sl_log.h"
#include "sv.h"
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

const char* ictx_stack_node_type_to_str(stack_node_type type) {
	switch(type) {
		case INTEGER:   return "INTEGER";
		case DOUBLE:    return "DOUBLE";
		case CHAR:      return "CHAR";
		case STRING:    return "SRTRING";
		case UNDEFINED: return "UNDEFINED";
	}
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
			case UNDEFINED:
				printf("%s\n", ictx_stack_node_type_to_str(UNDEFINED));
				break;
		}
	}
}

#define ARITH_OPERATION(l, r, resulttype, ltype, rtype, expr) \
	if (l.type == ltype && r.type == rtype) {\
		n.type = resulttype;\
		(expr);\
	}

void ictx_process_expression(interpreter_ctx* ictx, Expression* exp) {
	// =================
	// Term
	// =================
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
	// =================
	// StackOp
	// =================
	if (exp->type == EXPRESSION_TYPE_STACK_OP) {
		if (sv_eq(exp->StackOp.op.op, SV("."))) {
			ictx->stack_top--;
			return;
		}
		/**
		 *    Operation: ;
		 *    Function: Duplicate the top of the stack
		 */
		if (sv_eq(exp->EEO.operation.op, SV(";"))) {
			stack_node n = ictx->stack[ictx->stack_top];
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
	}

	// =================
	// ProcedureCall
	// =================
	if (exp->type == EXPRESSION_TYPE_PROC_CALL) {
		if (sv_eq(exp->EProcCall.proc_call.name, SV("print"))) {
			stack_node l = ictx->stack[ictx->stack_top];
			interp_builtin_print(l);
			return;
		}
		if (sv_eq(exp->EProcCall.proc_call.name, SV("println"))) {
			stack_node l = ictx->stack[ictx->stack_top];
			interp_builtin_println(l);
			return;
		}
		if (sv_eq(exp->EProcCall.proc_call.name, SV("input"))) {
			stack_node l = {0};
			interp_builtin_input(&l);
			ictx->stack[++ictx->stack_top] = l;
			return;
		}
		sl_assert(0, "Proc call for '" SV_Fmt "' not implemented", SV_Arg(exp->EProcCall.proc_call.name));
	}
	// =======================
	// EEO    (Expr, Expr, Op)
	// =======================
	if (exp->type == EXPRESSION_TYPE_EEO) {
		ictx_process_expression(ictx, exp->EEO.left);
		ictx_process_expression(ictx, exp->EEO.right);
		stack_node r = ictx->stack[ictx->stack_top--];
		stack_node l = ictx->stack[ictx->stack_top--];
#define UNDEFINED_OP_FMT "Undefined operation %c -> \n(left: %s, right: %s)\nLine %d, Col %d\n"

		/**
		 *   Operation: +
		 */
		if (sv_eq(exp->EEO.operation.op, SV("+"))) {
			stack_node n = (stack_node) {.type=UNDEFINED};
			ARITH_OPERATION(l, r, DOUBLE,  DOUBLE , DOUBLE,  n.doubleLiteral  = l.doubleLiteral  + r.doubleLiteral);
			ARITH_OPERATION(l, r, INTEGER, DOUBLE , DOUBLE,  n.doubleLiteral  = l.integerLiteral + r.doubleLiteral);
			ARITH_OPERATION(l, r, DOUBLE,  INTEGER, DOUBLE,  n.doubleLiteral  = l.doubleLiteral  + r.integerLiteral);
			ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral + r.integerLiteral);
			sl_assert(n.type != UNDEFINED, UNDEFINED_OP_FMT,
				'+',
				ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type),
				exp->state.line, exp->state.col
			);
			ictx->stack[++ictx->stack_top] = n;
			return;    // we've performed the operation. there is no need to continue in this function
		}
		/**
		 *   Operation: -
		 */
		if (sv_eq(exp->EEO.operation.op, SV("-"))) {
			stack_node n = (stack_node) {.type=UNDEFINED};
			ARITH_OPERATION(l, r, DOUBLE,  DOUBLE , DOUBLE,  n.doubleLiteral  = l.doubleLiteral  - r.doubleLiteral);
			ARITH_OPERATION(l, r, INTEGER, DOUBLE , DOUBLE,  n.doubleLiteral  = l.integerLiteral - r.doubleLiteral);
			ARITH_OPERATION(l, r, DOUBLE,  INTEGER, DOUBLE,  n.doubleLiteral  = l.doubleLiteral  - r.integerLiteral);
			ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral - r.integerLiteral);
			sl_assert(n.type != UNDEFINED, UNDEFINED_OP_FMT,
				'+',
				ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type),
				exp->state.line, exp->state.col
			);
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		/**
		 *   Operation: *
		 */
		if (sv_eq(exp->EEO.operation.op, SV("*"))) {
			stack_node n = (stack_node) {.type=UNDEFINED};
			ARITH_OPERATION(l, r, DOUBLE,  DOUBLE , DOUBLE,  n.doubleLiteral  = l.doubleLiteral  * r.doubleLiteral);
			ARITH_OPERATION(l, r, INTEGER, DOUBLE , DOUBLE,  n.doubleLiteral  = l.integerLiteral * r.doubleLiteral);
			ARITH_OPERATION(l, r, DOUBLE,  INTEGER, DOUBLE,  n.doubleLiteral  = l.doubleLiteral  * r.integerLiteral);
			ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral * r.integerLiteral);
			sl_assert(n.type != UNDEFINED, UNDEFINED_OP_FMT,
				'*',
				ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type),
				exp->state.line, exp->state.col
			);
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		/**
		 *   Operation: /
		 */
		if (sv_eq(exp->EEO.operation.op, SV("/"))) {
			stack_node n = (stack_node) {.type=UNDEFINED};
			ARITH_OPERATION(l, r, DOUBLE,  DOUBLE , DOUBLE,  n.doubleLiteral  = l.doubleLiteral  / r.doubleLiteral);
			ARITH_OPERATION(l, r, INTEGER, DOUBLE , DOUBLE,  n.doubleLiteral  = l.integerLiteral / r.doubleLiteral);
			ARITH_OPERATION(l, r, DOUBLE,  INTEGER, DOUBLE,  n.doubleLiteral  = l.doubleLiteral  / r.integerLiteral);
			ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral / r.integerLiteral);
			sl_assert(n.type != UNDEFINED, UNDEFINED_OP_FMT,
				'/',
				ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type),
				exp->state.line, exp->state.col
			);
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		/**
		 *   Operation: %
		 */
		if (sv_eq(exp->EEO.operation.op, SV("%"))) {
			stack_node n = (stack_node) {.type=UNDEFINED};
			ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral % r.integerLiteral);
			sl_assert(n.type != UNDEFINED, UNDEFINED_OP_FMT,
				'%',
				ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type),
				exp->state.line, exp->state.col
			);
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		/**
		 *   Operation: >
		 */
		if (sv_eq(exp->EEO.operation.op, SV(">"))) {
			stack_node n = (stack_node) {.type=UNDEFINED};
			ARITH_OPERATION(l, r, DOUBLE,  DOUBLE,  INTEGER, n.integerLiteral = l.doubleLiteral  > r.doubleLiteral);
			ARITH_OPERATION(l, r, INTEGER, DOUBLE,  INTEGER, n.integerLiteral = l.integerLiteral > r.doubleLiteral);
			ARITH_OPERATION(l, r, DOUBLE,  INTEGER, INTEGER, n.integerLiteral = l.doubleLiteral  > r.doubleLiteral);
			ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral > r.integerLiteral);
			sl_assert(n.type != UNDEFINED, UNDEFINED_OP_FMT,
				'>',
				ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type),
				exp->state.line, exp->state.col
			);
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		/**
		 *   Operation: <
		 */
		if (sv_eq(exp->EEO.operation.op, SV("<"))) {
			stack_node n = (stack_node) {.type=UNDEFINED};
			ARITH_OPERATION(l, r, DOUBLE,  DOUBLE,  INTEGER, n.integerLiteral = l.doubleLiteral  < r.doubleLiteral);
			ARITH_OPERATION(l, r, INTEGER, DOUBLE,  INTEGER, n.integerLiteral = l.integerLiteral < r.doubleLiteral);
			ARITH_OPERATION(l, r, DOUBLE,  INTEGER, INTEGER, n.integerLiteral = l.doubleLiteral  < r.doubleLiteral);
			ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral < r.integerLiteral);
			sl_assert(n.type != UNDEFINED, UNDEFINED_OP_FMT,
				'<',
				ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type),
				exp->state.line, exp->state.col
			);
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		/**
		 *   Operation: &&
		 */
		if (sv_eq(exp->EEO.operation.op, SV("&&"))) {
			stack_node n = (stack_node) {.type=UNDEFINED};
			ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral && r.integerLiteral);
			sl_assert(n.type != UNDEFINED, UNDEFINED_OP_FMT,
				'<',
				ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type),
				exp->state.line, exp->state.col
			);
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		/**
		 *   Operation: ||
		 */
		if (sv_eq(exp->EEO.operation.op, SV("||"))) {
			stack_node n = (stack_node) {.type=UNDEFINED};
			ARITH_OPERATION(l, r, INTEGER, INTEGER, INTEGER, n.integerLiteral = l.integerLiteral || r.integerLiteral);
			sl_assert(n.type != UNDEFINED, UNDEFINED_OP_FMT,
				'<',
				ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type),
				exp->state.line, exp->state.col
			);
			ictx->stack[++ictx->stack_top] = n;
			return;
		}

		sl_assert(0, "Undefined operation " SV_Fmt "\n", SV_Arg(exp->EEO.operation.op));
	}
}

void ictx_process_statement(interpreter_ctx* ictx, Statement* stmt) {
}

void ictx_process_stmt_expr(interpreter_ctx* ictx, StatementExpression stmtexpr) {
	switch (stmtexpr.type) {
		case STATEMENT_EXPR_TYPE_EXPRESSION:
			ictx_process_expression(ictx, stmtexpr.expr);
			break;
		case STATEMENT_EXPR_TYPE_STATEMENT:
			ictx_process_statement(ictx, stmtexpr.stmt);
			break;
	}
}

void ictx_run(interpreter_ctx* ictx, Program p) {
	// fprintf(stderr, "INTERPRETER OFFLINE\n");
	// return;

	for (AST_Node* n = cvector_begin(p.p); n != cvector_end(p.p); n++) {
		if (n->nodeType == AST_NODE_TYPE_STATEMENT_EXPRESSION) {
			ictx_process_stmt_expr(ictx, n->stmtExpr);
		}
	}
}
