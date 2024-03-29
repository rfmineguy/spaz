#include "parser.h"
#include "ast.h"
#include "ast_helper.h"
#include "interpreter.h"
#include <stdlib.h>
#include <stdio.h>

// parse_stack_node create_stack_node_terminal(token t) {
// 	parse_stack_node psn = {0};
// 	psn.type = PSNT_TERMINAL;
// 	psn.terminal = t;
// 	return psn;
// }

// parse_stack_node create_stack_node_nonterminal() {}

// const char* pctx_type_str(pstack_node_type nodetype) {
// 	switch (nodetype) {
// 		case PSNT_TERMINAL:       return "Terminal";
// 		case PSNT_TERM: 					return "Term";
// 		case PSNT_EXPRESSION:     return "Expression";
// 		case PSNT_PROCEDURE_CALL: return "ProcedureCall";
// 		case PSNT_PROCEDURE_DEF:  return "ProcedureDef";
// 		case PSNT_IFF: 						return "Iff";
// 		case PSNT_SWITCH: 				return "Switch";
// 		case PSNT_CASE: 					return "Case";
// 	}
// }

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

bool pctx_top_is(parse_ctx* pctx, AST_NodeType types[], int size) {
	// If there is nothing on the stack, how could it possibly be equal?
	if (pctx->pstack.length < size && pctx->pstack.top != -1)
		return false;

	int start = pctx->pstack.top;
	int end   = start - size;
	if (end < -1)
		return false;
	for (int i = start; i > end; i--) {
		if (types[i - pctx->pstack.top] != pctx->pstack.data[i].nodeType) {
			return false;
		}
	}

	return true;
}

void pctx_push(parse_ctx* pctx, AST_Node node) {
	if (pctx->pstack.length + 1 >= pctx->pstack.capacity) {
		pctx->pstack.data = realloc(pctx->pstack.data, pctx->pstack.capacity * 2);
		pctx->pstack.capacity *= 2;
	}
	pctx->pstack.data[pctx->pstack.top] = node;
	pctx->pstack.top++;
	pctx->pstack.length++;
}

AST_Node pctx_peek(parse_ctx* pctx) {
	if (pctx->pstack.length == 0) {
		return (AST_Node){}; // shouldn't happen. lol, famous last words
	}
	return pctx->pstack.data[pctx->pstack.top - 1];
}

void pctx_pop(parse_ctx* pctx) {
	if (pctx->pstack.length == 0) {
		fprintf(stderr, "Attempt to pop from empty stack\n");
		return; // shouldn't happen. lol, famous last words
	}
	pctx->pstack.top--;
	pctx->pstack.length--;
}

void pctx_pop_n(parse_ctx* pctx, int n) {
	for (int i = 0; i < n; i++) {
		pctx_pop(pctx);
	}
}

void pctx_print_stack(parse_ctx* pctx) {
	for (int i = 0; i < pctx->pstack.length; i++) {
		ast_print_node(pctx->pstack.data[i], 0);
	}
}

int try_convert_token_to_terminal(token tok, AST_Node* out_n) {
	AST_NodeType nt = AST_NODE_TYPE_UNDEFINED;
	Terminal t;
	int status = 0; // 0 indicates no reduction
	switch (tok.type) {
		case T_ID: 
			nt = AST_NODE_TYPE_TERMINAL;
			t = P_NEW_TERMINAL(TERMINAL_TYPE_IDENTIFIER, .id=tok.text);
			status = 1;
			break;
		case T_HEX_LIT:
			nt = AST_NODE_TYPE_TERMINAL;
			t = P_NEW_TERMINAL(TERMINAL_TYPE_HEX_LIT, .integer_lit=interpret_hex_sv_to_int(tok.text));
			status = 1;
			break;
		case T_DOUBLE_LIT:
			nt = AST_NODE_TYPE_TERMINAL;
			t=P_NEW_TERMINAL(TERMINAL_TYPE_DOUBLE_LIT, .dbl_lit=interpret_double_sv_to_double(tok.text));
			status = 1;
			break;
		case T_DECIMAL_LIT:
			nt = AST_NODE_TYPE_TERMINAL;
			t=P_NEW_TERMINAL(TERMINAL_TYPE_DEC_LIT, .integer_lit=interpret_decimal_sv_to_int(tok.text));
			status = 1;
			break;
		case T_STRING_LIT:
			nt = AST_NODE_TYPE_TERMINAL;
			t=P_NEW_TERMINAL(TERMINAL_TYPE_STRING_LIT, .str_lit=tok.text);
			status = 1;
			break;
		case T_CHAR_LIT:
			nt = AST_NODE_TYPE_TERMINAL;
			t=P_NEW_TERMINAL(TERMINAL_TYPE_CHAR_LIT, .chr_lit=tok.text);
			status = 1;
			break;
		case T_LOGIC_BEG...T_LOGIC_END:
			nt = AST_NODE_TYPE_TERMINAL;
			t=P_NEW_TERMINAL(TERMINAL_TYPE_LOGIC_OP, .operatorr=tok.text);
			status = 1;
			break;
		case T_STACK_BEG...T_STACK_END:
			nt = AST_NODE_TYPE_TERMINAL;
			t=P_NEW_TERMINAL(TERMINAL_TYPE_STACK_OP, .operatorr=tok.text);
			status = 1;
			break;
		case T_ARITH_BEG...T_ARITH_END:
			nt = AST_NODE_TYPE_TERMINAL;
			t=P_NEW_TERMINAL(TERMINAL_TYPE_ARITH_OP, .operatorr=tok.text);
			status = 1;
			break;
		case T_RESERVE_BEG...T_RESERVE_END:
			nt = AST_NODE_TYPE_TERMINAL;
			t=P_NEW_TERMINAL(TERMINAL_TYPE_RESERVED, .reserved=((Reserved) {.type=tok.type, .str=tok.text}));
			status = 1;
			break;
		default: break;
	}
	*out_n = (AST_Node) {.nodeType=nt, .terminal=t};
	return status;
}

int try_reduce(parse_ctx* pctx, AST_Node* out_n) {
	return 0;
}
