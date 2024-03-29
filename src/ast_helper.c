#include "ast_helper.h"
#include "ast.h"
#include <stdio.h>

void ast_print_node(AST_Node node, int depth) {
	switch (node.nodeType) {
	case AST_NODE_TYPE_UNDEFINED:      printf("Node {UNDEFINED}\n"); break;
	case AST_NODE_TYPE_RESERVED:       ast_print_reserved(node.reserved, depth + 1); break;
	case AST_NODE_TYPE_TERMINAL:       ast_print_terminal(node.terminal, depth + 1); break;
	case AST_NODE_TYPE_TERM:           ast_print_term(node.term, depth + 1); break;
	case AST_NODE_TYPE_OPERATOR:       ast_print_operator(node.op, depth + 1); break;
	case AST_NODE_TYPE_EXPRESSION:     ast_print_expression(node.expression, depth + 1); break;
	case AST_NODE_TYPE_PROCEDURE_DEF:  ast_print_procedure_def(node.procDef, depth + 1); break;
	case AST_NODE_TYPE_STATEMENT:      ast_print_statement(node.statement, depth + 1); break;
	case AST_NODE_TYPE_PROCEDURE_CALL: ast_print_procedure_call(node.procedureCall, depth + 1); break;
	case AST_NODE_TYPE_IFF: 					 ast_print_iff(node.iff, depth + 1); break;	
	case AST_NODE_TYPE_SWITCH:  			 ast_print_switch(node.switchf, depth + 1); break; 
	case AST_NODE_TYPE_CASE: 					 ast_print_switch_case(node.casef, depth + 1); break; 
	case AST_NODE_TYPE_BLOCK: 				 ast_print_block(node.block, depth + 1); break; 
	case AST_NODE_TYPE_SWITCH_BLOCK:   ast_print_switch_block(node.switchBlock, depth + 1); break;
	default:   												 assert(0 && "Unimplemented ast node type");
	}
}
void ast_print_reserved       (Reserved reserved, int depth) {
	printf("Reserved...\n");
}

void ast_print_terminal       (Terminal terminal, int depth) {
	printf("Terminal");
	switch (terminal.type) {
		case TERMINAL_TYPE_IDENTIFIER: printf(" |   Identifier = " SV_Fmt "\n", SV_Arg(terminal.id));         break;
		case TERMINAL_TYPE_DEC_LIT:    printf(" |   DecLit = %d\n", terminal.integer_lit);                    break;
		case TERMINAL_TYPE_DOUBLE_LIT: printf(" |   DblLit = %.8f\n", terminal.dbl_lit);                      break;
		case TERMINAL_TYPE_HEX_LIT:    printf(" |   HexLit = 0x%X\n", terminal.integer_lit);                  break;
		case TERMINAL_TYPE_STRING_LIT: printf(" |   StrLit = " SV_Fmt "\n", SV_Arg(terminal.str_lit));        break;
		case TERMINAL_TYPE_CHAR_LIT:   printf(" |   ChrLit = " SV_Fmt "\n", SV_Arg(terminal.chr_lit));        break;
		case TERMINAL_TYPE_LOGIC_OP:   printf(" |   LogicOp = " SV_Fmt "\n", SV_Arg(terminal.operatorr));     break;
		case TERMINAL_TYPE_STACK_OP:   printf(" |   StackOp = " SV_Fmt "\n", SV_Arg(terminal.operatorr)); 	  break;
		case TERMINAL_TYPE_ARITH_OP:   printf(" |   ArithOp = " SV_Fmt "\n", SV_Arg(terminal.operatorr)); 	  break;
		case TERMINAL_TYPE_RESERVED:   printf(" |   Reserved = " SV_Fmt "\n", SV_Arg(terminal.reserved.str)); break;
	}
}
void ast_print_term           (Term term, int depth) {
	printf("Term");
	switch (term.type) {
		case TERM_TYPE_DEC_LIT:    printf(" |   DecLit = %d\n", term._integer); break;
		case TERM_TYPE_DOUBLE_LIT: printf(" |   DblLit = %.8f\n", term._double); break;
		case TERM_TYPE_HEX_LIT:    printf(" |   HexLit = 0x%X\n", term._integer); break;
		case TERM_TYPE_STRING_LIT: printf(" |   StrLit = " SV_Fmt "\n", SV_Arg(term._string)); break;
		case TERM_TYPE_CHR_LIT:    printf(" |   ChrLit = " SV_Fmt "\n", SV_Arg(term._chr)); break;
	}
}
void ast_print_operator       (Operator op, int depth) {
	switch (op.type) {
		case OPERATOR_TYPE_ARITH: break;
		case OPERATOR_TYPE_LOGIC: break;
	}
}
void ast_print_expression     (Expression expr, int depth) {
	switch (expr.type) {
		case EXPRESSION_TYPE_EEO:      break;
		case EXPRESSION_TYPE_TERM:     break;
		case EXPRESSION_TYPE_STACK_OP: break;
	}
}
void ast_print_procedure_def  (ProcedureDef proc_def, int depth) {
}
void ast_print_statement      (Statement stmt, int depth) {
	switch (stmt.type) {
		case STATEMENT_TYPE_PROCEDURE_DEF:  break;
		case STATEMENT_TYPE_PROCEDURE_CALL: break;
		case STATEMENT_TYPE_IFF:            break;
		case STATEMENT_TYPE_SWITCH:         break;
		case STATEMENT_TYPE_CASE:         	break;
		case STATEMENT_TYPE_BLOCK:         	break;
	}
}
void ast_print_procedure_call (ProcedureCall proc_call, int depth) {
}
void ast_print_iff            (Iff iff, int depth) {
	// Print expression
	// Print block
}
void ast_print_switch         (Switch sswitch, int depth) {
}
void ast_print_switch_case    (SwitchCase ccase, int depth) {
}
void ast_print_block   				(Block block, int depth) {
}
void ast_print_switch_block   (SwitchBlock sw_block, int depth) {
}
