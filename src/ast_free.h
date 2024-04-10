#ifndef AST_FREE_H
#define AST_FREE_H
#include "ast.h"

void ast_free_node           (AST_Node);
void ast_free_program  			 (Program);
void ast_free_reserved       (Reserved);
void ast_free_terminal       (Terminal);
void ast_free_term           (Term);
void ast_free_operator       (Operator);
void ast_free_stmt_expr      (StatementExpression);
void ast_free_expression     (Expression*);
void ast_free_statement      (Statement*);
void ast_free_procedure_def  (ProcedureDef*);
void ast_free_procedure_call (ProcedureCall*);
void ast_free_iff            (Iff);
void ast_free_switch         (Switch*);
void ast_free_switch_case    (SwitchCase*);
void ast_free_block   			 (Block);
void ast_free_switch_block   (SwitchBlock*);

#endif
