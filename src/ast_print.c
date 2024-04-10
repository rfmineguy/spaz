#include "ast_print.h"
#include "ast.h"
#include "sl_assert.h"
#include "sl_log.h"
#include <stdio.h>

void ast_print_node_lite(AST_Node node) {
	switch (node.nodeType) {
		case AST_NODE_TYPE_UNDEFINED:
			sl_log_ast("Lite: Undefined"); break;
		case AST_NODE_TYPE_PROGRAM:
			sl_log_ast("Lite: Program"); break;
		case AST_NODE_TYPE_RESERVED:
			sl_log_ast("Lite: Reserved: " SV_Fmt "\"", SV_Arg(node.reserved.token.text)); break;
		case AST_NODE_TYPE_TERMINAL:
			sl_log_ast("Lite: Terminal"); break;
		case AST_NODE_TYPE_TERM:
			sl_log_ast("Lite: Term"); break;
		case AST_NODE_TYPE_OPERATOR:
			sl_log_ast("Lite: Operator"); break;
		case AST_NODE_TYPE_STATEMENT_EXPRESSION:
			sl_log_ast("Lite: StatementExpression"); break;
		case AST_NODE_TYPE_PROCEDURE_DEF:
			sl_log_ast("Lite: ProcedureDef"); break;
		case AST_NODE_TYPE_PROCEDURE_CALL:
			sl_log_ast("Lite: ProcedureCall"); break;
		case AST_NODE_TYPE_IFF:
			sl_log_ast("Lite: Iff"); break;
		case AST_NODE_TYPE_SWITCH:
			sl_log_ast("Lite: Switch"); break;
		case AST_NODE_TYPE_CASE:
			sl_log_ast("Lite: Case"); break;
		case AST_NODE_TYPE_BLOCK:
			sl_log_ast("Lite: Block"); break;
		case AST_NODE_TYPE_SWITCH_BLOCK:
			sl_log_ast("Lite: SwitchBlock"); break;
	}
}

void ast_print_node(AST_Node node, int depth) {
	switch (node.nodeType) {
	case AST_NODE_TYPE_UNDEFINED:      			sl_log_ast("Node {UNDEFINED}"); break;
	case AST_NODE_TYPE_PROGRAM: 			 			ast_print_program(node.program, depth); break;
	case AST_NODE_TYPE_RESERVED:       			ast_print_reserved(node.reserved, depth); break;
	case AST_NODE_TYPE_TERMINAL:       			ast_print_terminal(node.terminal, depth); break;
	case AST_NODE_TYPE_TERM:           			ast_print_term(node.term, depth); break;
	case AST_NODE_TYPE_OPERATOR:       			ast_print_operator(node.op, depth); break;
	case AST_NODE_TYPE_PROCEDURE_DEF:  			ast_print_procedure_def(node.procDef, depth); break;
	case AST_NODE_TYPE_STATEMENT_EXPRESSION:ast_print_stmt_expr(node.stmtExpr, depth); break;
	// case AST_NODE_TYPE_STATEMENT:           ast_print_statement(node.statement, depth); break;
	// case AST_NODE_TYPE_PROCEDURE_CALL:      ast_print_procedure_call(node.procedureCall, depth); break;
	case AST_NODE_TYPE_IFF: 					      ast_print_iff(node.iff, depth); break;	
	case AST_NODE_TYPE_SWITCH:  			      ast_print_switch(node.switchf, depth); break; 
	case AST_NODE_TYPE_CASE: 					      ast_print_switch_case(node.casef, depth); break; 
	case AST_NODE_TYPE_BLOCK: 				      ast_print_block(node.block, depth); break; 
	case AST_NODE_TYPE_SWITCH_BLOCK:        ast_print_switch_block(node.switchBlock, depth); break;
	default:   												      sl_assert(0, "Unimplemented ast node type [%d]", node.nodeType);
	}
}

void ast_print_program  			(Program prog, int depth) {
	sl_log_ast("%*cProgram: \n", depth * 2, ' ');
	for (AST_Node* it = cvector_begin(prog.p); it != cvector_end(prog.p); it++) {
		ast_print_node(*it, depth+1);
	}
}

void ast_print_reserved       (Reserved reserved, int depth) {
	sl_log_ast("%*cReserved: \'" SV_Fmt "\'", depth * 2, ' ', SV_Arg(reserved.token.text));
}

void ast_print_terminal       (Terminal terminal, int depth) {
	sl_log_ast("%*cTerminal:\n", depth * 2, ' ');
	switch (terminal.type) {
		case TERMINAL_TYPE_IDENTIFIER: sl_log_ast(" |   Identifier = " SV_Fmt, SV_Arg(terminal.id));         break;
		case TERMINAL_TYPE_DEC_LIT:    sl_log_ast(" |   DecLit = %d", terminal.integer_lit);                    break;
		case TERMINAL_TYPE_DOUBLE_LIT: sl_log_ast(" |   DblLit = %.8f", terminal.dbl_lit);                      break;
		case TERMINAL_TYPE_HEX_LIT:    sl_log_ast(" |   HexLit = 0x%X", terminal.integer_lit);                  break;
		case TERMINAL_TYPE_STRING_LIT: sl_log_ast(" |   StrLit = " SV_Fmt, SV_Arg(terminal.str_lit));        break;
		case TERMINAL_TYPE_CHAR_LIT:   sl_log_ast(" |   ChrLit = " SV_Fmt, SV_Arg(terminal.chr_lit));        break;
	}
}
void ast_print_term           (Term term, int depth) {
	sl_log_ast("%*cTerm:", depth * 2, ' ');
	switch (term.type) {
		case TERM_TYPE_DEC_LIT:    sl_log_ast("%*cDecLit = %d",         (depth + 1) * 2, ' ', term._integer); break;
		case TERM_TYPE_DOUBLE_LIT: sl_log_ast("%*cDblLit = %.8f",       (depth + 1) * 2, ' ', term._double); break;
		case TERM_TYPE_HEX_LIT:    sl_log_ast("%*cHexLit = 0x%X",       (depth + 1) * 2, ' ', term._integer); break;
		case TERM_TYPE_STRING_LIT: sl_log_ast("%*cStrLit = " SV_Fmt "", (depth + 1) * 2, ' ', SV_Arg(term._string)); break;
		case TERM_TYPE_CHR_LIT:    sl_log_ast("%*cChrLit = " SV_Fmt "", (depth + 1) * 2, ' ', SV_Arg(term._chr)); break;
	}                                                                 
}
void ast_print_operator       (Operator op, int depth) {
	sl_log_ast("%*cOperator: ", depth * 2, ' ');
	switch (op.type) {
		case OPERATOR_TYPE_ARITH: sl_log_ast("%*cArithOp = " SV_Fmt "", (depth + 1) * 2, ' ', SV_Arg(op.op)); break;
		case OPERATOR_TYPE_LOGIC: sl_log_ast("%*cLogicOp = " SV_Fmt "", (depth + 1) * 2, ' ', SV_Arg(op.op)); break;
		case OPERATOR_TYPE_STACK: sl_log_ast("%*cStackOp = " SV_Fmt "", (depth + 1) * 2, ' ', SV_Arg(op.op)); break;
		default: sl_log_ast("%*cUnknown: %d", (depth + 1) * 2, ' ', op.type);
	}
}
void ast_print_stmt_expr(StatementExpression stmtExpr, int depth) {
	sl_log_ast("%*cStmtExpr: ", depth * 2, ' ');
	switch (stmtExpr.type) {
		case STATEMENT_EXPR_TYPE_EXPRESSION: ast_print_expression(stmtExpr.expr, depth + 1); break;
		case STATEMENT_EXPR_TYPE_STATEMENT: ast_print_statement(stmtExpr.stmt, depth + 1); break;
	}
}
void ast_print_expression     (Expression *expr, int depth) {
	switch (expr->type) {
		case EXPRESSION_TYPE_EEO:
			sl_log_ast("%*cExpression(EEO): ", depth * 2, ' ');
			ast_print_expression(expr->EEO.left, depth + 1);
			ast_print_expression(expr->EEO.right, depth + 1);
			ast_print_operator(expr->EEO.operation, depth + 1);
			break;
		case EXPRESSION_TYPE_TERM:
			sl_log_ast("%*cExpression(Term): ", depth * 2, ' ');
			ast_print_term(expr->ETerm.term, depth + 1);
			break;
		case EXPRESSION_TYPE_STACK_OP:
			/* ast_print_operator(expr->EEO.operation, depth + 1); */
			break;
		case EXPRESSION_TYPE_PROC_CALL:
			sl_log_ast("%*cExpression(ProcCall): ", depth * 2, ' ');
			ast_print_procedure_call(&expr->EProcCall.proc_call, depth + 1);
			break;
	}
}
void ast_print_procedure_def  (ProcedureDef *proc_def, int depth) {
}
void ast_print_statement      (Statement *stmt, int depth) {
	sl_log_ast("%*cStatement (Print not implemented)", depth * 2, ' ');
	switch (stmt->type) {
		case STATEMENT_TYPE_PROCEDURE_DEF:  break;
		// case STATEMENT_TYPE_PROCEDURE_CALL: break;
		case STATEMENT_TYPE_IFF:            break;
		case STATEMENT_TYPE_SWITCH:         break;
		case STATEMENT_TYPE_CASE:         	break;
		case STATEMENT_TYPE_BLOCK:         	break;
	}
}
void ast_print_procedure_call (ProcedureCall *proc_call, int depth) {
	if (!proc_call)
		return;
	sl_log_ast("%*cProcedureCall: ", depth * 2, ' ');
	sl_log_ast("%*cName: " SV_Fmt "", (depth + 1) * 2, ' ', SV_Arg(proc_call->name));
}
void ast_print_iff            (Iff *iff, int depth) {
	// Print expression
	// Print block
}
void ast_print_switch         (Switch *sswitch, int depth) {
}
void ast_print_switch_case    (SwitchCase *ccase, int depth) {
}
void ast_print_block   				(Block block, int depth) {
	sl_log_ast("%*cBlock: [%lu]", depth * 2, ' ', cvector_size(block.items));
	for (StatementExpression* it = cvector_begin(block.items); it != cvector_end(block.items); it++) {
		switch (it->type) {
			case STATEMENT_EXPR_TYPE_EXPRESSION: ast_print_expression(it->expr, depth + 1); break;
			case STATEMENT_EXPR_TYPE_STATEMENT: ast_print_statement(it->stmt, depth + 1); break;
			default: sl_assert(0, "Something went very wrong");
		}
	}
}
void ast_print_switch_block   (SwitchBlock *sw_block, int depth) {
}
