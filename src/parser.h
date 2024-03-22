#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include "tokenizer.h"

#define PSNODE_NEW_TERMINAL(t) (parse_stack_node) {.type=PSNT_TERMINAL, .terminal=t}
#define PSNODE_NEW_TERM(type, v) (parse_stack_node) {}
#define NT_LIST(...) (pstack_node_type[]) { __VA_ARGS__ }

typedef enum {
	PSNT_TERMINAL, PSNT_TERM, PSNT_EXPRESSION, PSNT_PROCEDURE_CALL, PSNT_PROCEDURE_DEF, PSNT_IFF, PSNT_SWITCH, PSNT_CASE
} pstack_node_type ;
typedef struct parse_stack_node {
	pstack_node_type type;
	union {
		token terminal;
		Term term;
		Expression expression;
		ProcedureCall procedureCall;
		ProcedureDef procedureDef;
		Iff iff;
		Switch switchh;
		Case casee;
	};
} parse_stack_node;

/* Should this stack be linked list based?
 *   - this would eliminate the issue of potentially overruninning the stack (though this would most likely be rare?)
 * 	 - 
 */
typedef struct {
	parse_stack_node *data;
	int capacity, length, top;
} stack;

typedef struct {
	stack pstack;
} parse_ctx;

parse_stack_node  create_stack_node_terminal(token);

const char*       pctx_type_str(pstack_node_type);
parse_ctx         pctx_new(int);
void  						pctx_free(parse_ctx*);
bool   					  pctx_top_is(parse_ctx*, pstack_node_type[], int);
void   					  pctx_push(parse_ctx*, parse_stack_node);
parse_stack_node  pctx_peek(parse_ctx*);
void 						 	pctx_pop(parse_ctx*);

#endif
