#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include "tokenizer.h"

#define PSNODE_NEW_TERMINAL(t) (parse_stack_node) {.node.type=PSNT_TERMINAL, .terminal=t}
#define PSNODE_NEW_TERM(ttype, v) (parse_stack_node) {.node.type=PSNT_TERM, .term=(Term){.type=(ttype), (v)}}
#define NT_LIST(...) (AST_NodeType[]) { __VA_ARGS__ }

#define P_NEW_TERMINAL(type_, expr) \
	(Terminal) {.type=type_, expr}
#define P_NEW_TERM(type_, expr) \
	(Term) {.type=type_, expr}

typedef struct parse_stack_node {
	AST_Node node;
} parse_stack_node;

typedef struct {
	AST_Node *data;
	int capacity, length, top;
} stack;

typedef struct {
	stack pstack;
} parse_ctx;

parse_stack_node  create_stack_node_terminal(token);

// Initialization/Destruction
parse_ctx         pctx_new(int);
void  						pctx_free(parse_ctx*);

// Stack operations
bool   					  pctx_top_is(parse_ctx*, AST_NodeType[], int);
void   					  pctx_push(parse_ctx*, AST_Node);
AST_Node          pctx_peek(parse_ctx*);
AST_Node          pctx_peek_offset(parse_ctx*, int);
void 						 	pctx_pop(parse_ctx*);
void 							pctx_pop_n(parse_ctx*, int);
void 							pctx_print_stack(parse_ctx*);

// Parse logic
// Params:
//   - token   :  token to reduce
//   - AST_Node:  pointer to AST_Node to populate
// Return:
//   - number of matched nodes
//   - 0 indicates that nothing was reduced
int               try_convert_token_to_terminal(token, AST_Node*);
int 							try_reduce(parse_ctx*, AST_Node*);
#endif
