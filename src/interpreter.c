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
		case STRING:    return "STRING";
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


/*
 *  A helper struct for using the ARITH_OPERATION macro
 *    + resultType  -> what is the type of the resulting operation?
 *    + leftType    -> what is the type of the left operand?
 *    + rightType   -> what is the type of the right operand?
 */
typedef struct ArithInfo {
	stack_node_type resultType;
	stack_node_type leftType, rightType;
} ArithInfo;

/*
 *  A macro used to perform an arbitrary operation based on the two operand types
 *    + l         -> the left operand (stack_node)
 *    + r         -> the right operand (stack_node)
 *    + arithInfo -> instance of the struct defined above
 *    + expr      -> the operation to execute if the left and right operands pass type checking
 */
#define ARITH_OPERATION(l, r, arithInfo, expr) \
	if (l.type == arithInfo.leftType && r.type == arithInfo.rightType) {\
		n.type = arithInfo.resultType;\
		{\
			(expr);\
		}\
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
		/**
		 *    Operation: .
		 *    Function: Peek the top of the stack
		 */
		if (sv_eq(exp->StackOp.op.op, SV(","))) {
			ictx->peeked = ictx->stack[ictx->stack_top];
			//sl_log("top of stack: %d", ictx->stack[ictx->stack_top].type);
			return;
		}
		/**
		 *    Operation: .
		 *    Function: Pop the top of the stack
		 */
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

		stack_node n = (stack_node) {.type=UNDEFINED};
		if (sv_eq(exp->EEO.operation.op, SV("+"))) {
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType=DOUBLE,  .leftType=DOUBLE,  .rightType=DOUBLE}),  n.doubleLiteral  = (l.doubleLiteral  + r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType=DOUBLE,  .leftType=INTEGER, .rightType=DOUBLE}),  n.doubleLiteral  = (l.integerLiteral + r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType=DOUBLE,  .leftType=DOUBLE,  .rightType=DOUBLE}),  n.doubleLiteral  = (l.doubleLiteral  + r.integerLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType=INTEGER, .leftType=INTEGER, .rightType=INTEGER}), n.integerLiteral = (l.integerLiteral + r.integerLiteral));
			sl_assert(n.type != UNDEFINED, "Operator '+' not defined for %s and %s\n", ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type));
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		if (sv_eq(exp->EEO.operation.op, SV("-"))) {
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = DOUBLE , .leftType = DOUBLE , .rightType = DOUBLE}),  n.doubleLiteral  = (l.doubleLiteral  - r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = DOUBLE , .leftType = INTEGER, .rightType = DOUBLE}),  n.doubleLiteral  = (l.integerLiteral - r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = DOUBLE , .leftType = DOUBLE,  .rightType = INTEGER}), n.doubleLiteral  = (l.doubleLiteral  - r.integerLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = INTEGER, .rightType = INTEGER}), n.integerLiteral = (l.integerLiteral - r.integerLiteral));
			sl_assert(n.type != UNDEFINED, "Operator '-' not defined for %s and %s\n", ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type));
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		if (sv_eq(exp->EEO.operation.op, SV("*"))) {
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = DOUBLE , .leftType = DOUBLE , .rightType = DOUBLE}),  n.doubleLiteral  = (l.doubleLiteral  * r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = DOUBLE , .leftType = INTEGER, .rightType = DOUBLE}),  n.doubleLiteral  = (l.integerLiteral * r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = DOUBLE , .leftType = DOUBLE,  .rightType = INTEGER}), n.doubleLiteral  = (l.doubleLiteral  * r.integerLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = INTEGER, .rightType = INTEGER}), n.integerLiteral = (l.integerLiteral * r.integerLiteral));
			sl_assert(n.type != UNDEFINED, "Operator '*' not defined for %s and %s\n", ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type));
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		if (sv_eq(exp->EEO.operation.op, SV("/"))) {
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = DOUBLE , .leftType = DOUBLE , .rightType = DOUBLE}),  n.doubleLiteral  = (l.doubleLiteral  / r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = DOUBLE , .leftType = INTEGER, .rightType = DOUBLE}),  n.doubleLiteral  = (l.integerLiteral / r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = DOUBLE , .leftType = DOUBLE,  .rightType = INTEGER}), n.doubleLiteral  = (l.doubleLiteral  / r.integerLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = INTEGER, .rightType = INTEGER}), n.integerLiteral = (l.integerLiteral / r.integerLiteral));
			sl_assert(n.type != UNDEFINED, "Operator '/' not defined for %s and %s\n", ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type));
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		if (sv_eq(exp->EEO.operation.op, SV("%"))) {
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = INTEGER, .rightType = INTEGER}), n.integerLiteral = (l.integerLiteral % r.integerLiteral));
			sl_assert(n.type != UNDEFINED, "Operator '%%' not defined for %s and %s\n", ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type));
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		if (sv_eq(exp->EEO.operation.op, SV(">"))) {
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = DOUBLE , .rightType = DOUBLE}),  n.doubleLiteral  = (l.doubleLiteral  > r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = INTEGER, .rightType = DOUBLE}),  n.doubleLiteral  = (l.integerLiteral > r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = DOUBLE,  .rightType = INTEGER}), n.doubleLiteral  = (l.doubleLiteral  > r.integerLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = INTEGER, .rightType = INTEGER}), n.integerLiteral = (l.integerLiteral > r.integerLiteral));
			sl_assert(n.type != UNDEFINED, "Operator '>' not defined for %s and %s\n", ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type));
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		if (sv_eq(exp->EEO.operation.op, SV("<"))) {
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = DOUBLE , .rightType = DOUBLE}),  n.doubleLiteral  = (l.doubleLiteral  < r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = INTEGER, .rightType = DOUBLE}),  n.doubleLiteral  = (l.integerLiteral < r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = DOUBLE,  .rightType = INTEGER}), n.doubleLiteral  = (l.doubleLiteral  < r.integerLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = INTEGER, .rightType = INTEGER}), n.integerLiteral = (l.integerLiteral < r.integerLiteral));
			sl_assert(n.type != UNDEFINED, "Operator '<' not defined for %s and %s\n", ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type));
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		if (sv_eq(exp->EEO.operation.op, SV("&&"))) {
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER , .leftType = INTEGER , .rightType = INTEGER}),  n.integerLiteral  = (l.integerLiteral && r.integerLiteral));
			sl_assert(n.type != UNDEFINED, "Operator '&&' not defined for %s and %s\n", ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type));
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		if (sv_eq(exp->EEO.operation.op, SV("||"))) {
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER , .leftType = INTEGER , .rightType = INTEGER}),  n.integerLiteral  = (l.integerLiteral || r.integerLiteral));
			sl_assert(n.type != UNDEFINED, "Operator '||' not defined for %s and %s\n", ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type));
			ictx->stack[++ictx->stack_top] = n;
			return;
		}
		if (sv_eq(exp->EEO.operation.op, SV("=="))) {
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER , .leftType = DOUBLE , .rightType = DOUBLE}),  n.doubleLiteral  = (l.doubleLiteral  == r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER , .leftType = INTEGER, .rightType = DOUBLE}),  n.doubleLiteral  = (l.integerLiteral == r.doubleLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER , .leftType = DOUBLE,  .rightType = INTEGER}), n.doubleLiteral  = (l.doubleLiteral  == r.integerLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER, .leftType = INTEGER, .rightType = INTEGER}), n.integerLiteral = (l.integerLiteral == r.integerLiteral));
			ARITH_OPERATION(l, r, ((ArithInfo){.resultType = INTEGER,  .leftType = STRING,  .rightType = STRING}),  ({
				String_View l2 = l.stringLiteral, r2 = r.stringLiteral;
				int l2_quoted = l.stringLiteral.data[0] == '\"' && l.stringLiteral.data[l.stringLiteral.count - 1] == '\"';
				int r2_quoted = r.stringLiteral.data[0] == '\"' && r.stringLiteral.data[r.stringLiteral.count - 1] == '\"';
				if (l2_quoted) l2 = sv_from_parts(l.stringLiteral.data + 1, l.stringLiteral.count - 2);
				if (r2_quoted) r2 = sv_from_parts(r.stringLiteral.data + 1, r.stringLiteral.count - 2);
				n.integerLiteral = sv_eq(l2, r2);
				//sl_log(SV_Fmt " == " SV_Fmt " = %d\n", SV_Arg(l2), SV_Arg(r2), n.integerLiteral);
			}));
			sl_assert(n.type != UNDEFINED, "Operator '==' not defined for %s and %s\n", ictx_stack_node_type_to_str(l.type), ictx_stack_node_type_to_str(r.type));
			ictx->stack[++ictx->stack_top] = n;
			// sl_log("'==' Push: %d\n", ictx->stack[ictx->stack_top].integerLiteral);
			return;
		}
		sl_assert(0, "Undefined operation \"" SV_Fmt "\"\n", SV_Arg(exp->EEO.operation.op));
	}
}

void ictx_process_iff(interpreter_ctx* ictx, Iff iff) {
	//sl_log("iff");
	ictx_process_expression(ictx, iff.expression);
	if (ictx->stack[ictx->stack_top].type == INTEGER) {
		// sl_log("Stacktop: %d\n", ictx->stack[ictx->stack_top].integerLiteral);
		if (ictx->stack[ictx->stack_top].integerLiteral != 0) {
			ictx_process_block(ictx, iff.block);
		}
	}
}

void ictx_process_block(interpreter_ctx* ictx, Block block) {
	for (StatementExpression* it = cvector_begin(block.items); it != cvector_end(block.items); it++) {
		ictx_process_stmt_expr(ictx, *it);
	}
}

void ictx_process_statement(interpreter_ctx* ictx, Statement* stmt) {
	if (stmt->type == STATEMENT_TYPE_IFF) {
		ictx_process_iff(ictx, stmt->iff);
	}
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
