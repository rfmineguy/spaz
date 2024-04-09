#include "ast_free.h"
#include "ast.h"
#include "cvector.h"
#include "sl_assert.h"
#include <stdio.h>

int free_depth = 0;

#if defined(DEBUG) && DEBUG==1
#define BEGIN_FREE_FUNC { printf("BEGIN: %*c%s\n", free_depth * 2, ' ', __FUNCTION__); free_depth++; }
#define END_FREE_FUNC	{ printf("END: %*c%s\n", free_depth * 2, ' ', __FUNCTION__); free_depth--; }
#else
#define BEGIN_FREE_FUNC {}
#define END_FREE_FUNC {}
#endif

void ast_free_node           (AST_Node n){
	switch (n.nodeType) {
	case AST_NODE_TYPE_UNDEFINED:      			break;
	case AST_NODE_TYPE_PROGRAM: 			 			ast_free_program(n.program); break;
	case AST_NODE_TYPE_RESERVED:       			ast_free_reserved(n.reserved); break;
	case AST_NODE_TYPE_TERMINAL:       			ast_free_terminal(n.terminal); break;
	case AST_NODE_TYPE_TERM:           			ast_free_term(n.term); break;
	case AST_NODE_TYPE_OPERATOR:       			ast_free_operator(n.op); break;
	case AST_NODE_TYPE_PROCEDURE_DEF:  			ast_free_procedure_def(n.procDef); break;
	case AST_NODE_TYPE_STATEMENT_EXPRESSION:ast_free_stmt_expr(n.stmtExpr); break;
	// case AST_NODE_TYPE_PROCEDURE_CALL:      ast_free_procedure_call(n.procedureCall); break;
	case AST_NODE_TYPE_IFF: 					      ast_free_iff(n.iff); break;	
	case AST_NODE_TYPE_SWITCH:  			      ast_free_switch(n.switchf); break; 
	case AST_NODE_TYPE_CASE: 					      ast_free_switch_case(n.casef); break; 
	case AST_NODE_TYPE_BLOCK: 				      ast_free_block(n.block); break; 
	case AST_NODE_TYPE_SWITCH_BLOCK:        ast_free_switch_block(n.switchBlock); break;
	default:   												      assert(0 && "Unimplemented ast node type");
	}
}
void ast_free_program  			 (Program n){
	BEGIN_FREE_FUNC
	for (AST_Node* it = cvector_begin(n.p); it != cvector_end(n.p); it++) {
		ast_free_node(*it);
	}
	cvector_free(n.p);
	END_FREE_FUNC
}
void ast_free_reserved       (Reserved n){
	BEGIN_FREE_FUNC
	// Nothing to free for Reserved AST_Node
	END_FREE_FUNC
}
void ast_free_terminal       (Terminal n){
	BEGIN_FREE_FUNC
	// Nothing to free for Terminal AST_Node
	END_FREE_FUNC
}
void ast_free_term           (Term n){
	BEGIN_FREE_FUNC
	// Nothing to free for Terminal AST_Node
	END_FREE_FUNC
}
void ast_free_operator       (Operator n){
	BEGIN_FREE_FUNC
	// Nothing to free for Terminal AST_Node
	END_FREE_FUNC
}
void ast_free_stmt_expr      (StatementExpression n){
	BEGIN_FREE_FUNC
	switch (n.type) {
		case STATEMENT_EXPR_TYPE_STATEMENT:  ast_free_statement(n.stmt); break;
		case STATEMENT_EXPR_TYPE_EXPRESSION: ast_free_expression(n.expr); break;
	}
	END_FREE_FUNC
}
void ast_free_expression     (Expression* n){
	BEGIN_FREE_FUNC
	switch (n->type) {
		case EXPRESSION_TYPE_EEO: 
			ast_free_expression(n->EEO.left);
			ast_free_expression(n->EEO.right);
			free(n);
			break;
		case EXPRESSION_TYPE_TERM:
			free(n);
			// Nothing to free for Expression::Term
			break;
		case EXPRESSION_TYPE_PROC_CALL:
			ast_free_procedure_call(&n->EProcCall.proc_call);
			free(n);
			break;
		case EXPRESSION_TYPE_STACK_OP:
			// Doesn't exist...
			break;
	}
	END_FREE_FUNC
}
void ast_free_statement      (Statement* n){
	BEGIN_FREE_FUNC
	sl_assert(0, "statement free not implemented");
	END_FREE_FUNC
}
void ast_free_procedure_def  (ProcedureDef* n){
	BEGIN_FREE_FUNC
	sl_assert(0, "procedure_def free not implemented");
	END_FREE_FUNC
}
void ast_free_procedure_call (ProcedureCall* n){
	BEGIN_FREE_FUNC
	// Nothing to free for Expression::Term
	END_FREE_FUNC
}
void ast_free_iff            (Iff* n){
	BEGIN_FREE_FUNC
	sl_assert(0, "iff free not implemented");
	END_FREE_FUNC
}
void ast_free_switch         (Switch* n){
	BEGIN_FREE_FUNC
	sl_assert(0, "switch free not implemented");
	END_FREE_FUNC
}
void ast_free_switch_case    (SwitchCase* n){
	BEGIN_FREE_FUNC
	sl_assert(0, "switch_case free not implemented");
	END_FREE_FUNC
}
void ast_free_block   				(Block n){
	BEGIN_FREE_FUNC;
	for (StatementExpression* it = cvector_begin(n.items); it != cvector_end(n.items); it++) {
		ast_free_stmt_expr(*it);
	}
	cvector_free(n.items);
	END_FREE_FUNC;
}
void ast_free_switch_block   (SwitchBlock* n){
	BEGIN_FREE_FUNC
	sl_assert(0, "switch block free not implemented");
	END_FREE_FUNC
}
