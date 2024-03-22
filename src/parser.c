#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

parse_stack_node create_stack_node_terminal(token t) {
	parse_stack_node psn = {0};
	psn.type = PSNT_TERMINAL;
	psn.terminal = t;
	return psn;
}

parse_stack_node create_stack_node_nonterminal() {}

const char* pctx_type_str(pstack_node_type nodetype) {
	switch (nodetype) {
		case PSNT_TERMINAL:       return "Terminal";
		case PSNT_TERM: 					return "Term";
		case PSNT_EXPRESSION:     return "Expression";
		case PSNT_PROCEDURE_CALL: return "ProcedureCall";
		case PSNT_PROCEDURE_DEF:  return "ProcedureDef";
		case PSNT_IFF: 						return "Iff";
		case PSNT_SWITCH: 				return "Switch";
		case PSNT_CASE: 					return "Case";
	}
}

parse_ctx pctx_new(int initial_capacity) {
	parse_ctx ctx = {0};
	ctx.pstack.top = -1;
	ctx.pstack.length = 0;
	ctx.pstack.data = calloc(initial_capacity, sizeof(parse_stack_node));
	ctx.pstack.capacity = initial_capacity;
	return ctx;
}

void pctx_free(parse_ctx *pctx) {
	free(pctx->pstack.data);
	pctx->pstack.data = NULL;
}

bool pctx_top_is(parse_ctx* pctx, pstack_node_type types[], int size) {
	// If there is nothing on the stack, how could it possibly be equal?
	if (pctx->pstack.length < size && pctx->pstack.top != -1)
		return false;

	int start = pctx->pstack.top;
	int end   = start - size;
	if (end < -1)
		return false;
	for (int i = start; i > end; i--) {
		if (types[i - pctx->pstack.top] != pctx->pstack.data[i].type) {
			return false;
		}
	}

	return true;
}

void pctx_push(parse_ctx* pctx, parse_stack_node node) {
	if (pctx->pstack.length + 1 >= pctx->pstack.capacity) {
		pctx->pstack.data = realloc(pctx->pstack.data, pctx->pstack.capacity * 2);
		pctx->pstack.capacity *= 2;
	}
	pctx->pstack.data[pctx->pstack.top] = node;
	pctx->pstack.top++;
	pctx->pstack.length++;
}

parse_stack_node pctx_peek(parse_ctx* pctx) {
	if (pctx->pstack.length == 0) {
		return (parse_stack_node){}; // shouldn't happen. lol, famous last words
	}
	return pctx->pstack.data[pctx->pstack.top - 1];
}

void pctx_pop(parse_ctx* pctx) {
	if (pctx->pstack.length == 0) {
		return; // shouldn't happen. lol, famous last words
	}
	pctx->pstack.top--;
	pctx->pstack.length--;
}
