#ifndef AST_HELPER_H
#define AST_HELPER_H
#include "ast.h"

void ast_print_node_lite      (AST_Node);
void ast_print_node           (AST_Node, int);
void ast_print_program  			(Program, int);
void ast_print_reserved       (Reserved, int);
void ast_print_terminal       (Terminal, int);
void ast_print_term           (Term, int);
void ast_print_operator       (Operator, int);
void ast_print_stmt_expr      (StatementExpression, int);
void ast_print_expression     (Expression*, int);
void ast_print_statement      (Statement*, int);
void ast_print_procedure_def  (ProcedureDef*, int);
void ast_print_procedure_call (ProcedureCall*, int);
void ast_print_iff            (Iff, int);
void ast_print_switch         (Switch*, int);
void ast_print_switch_case    (SwitchCase*, int);
void ast_print_block   				(Block, int);
void ast_print_switch_block   (SwitchBlock*, int);

#endif
