#include "parser.h"
#include "ast.h"
#include "ast_print.h"
#include "cvector.h"
#include "sl_assert.h"
#include "sl_log.h"
#include "tokenizer.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

// @Deprecated
bool pctx_top_is(parse_ctx* pctx, AST_NodeType types[], int size) {
	// If there is nothing on the stack, how could it possibly be equal?
	if (pctx->pstack.length < size && pctx->pstack.top != -1) {
		fprintf(stderr, "Stack not big enough to check %d\n", size);
		// Debugging print
		// for (int i = 0; i < size; i++) {
		// 	printf("%d\n", types[i]);
		// }
		return false;
	}

	int start = pctx->pstack.top;
	int end   = start - size;
	if (end < -1) {
		return false;
	}
	for (int i = start; i > end; i--) {
		if (types[i - pctx->pstack.top] != pctx->pstack.data[size - i].nodeType) {
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
	pctx->pstack.top++;     // must increment first as top starts at -1
	pctx->pstack.length++;
	pctx->pstack.data[pctx->pstack.top] = node;
}

AST_Node pctx_peek(parse_ctx* pctx) {
	if (pctx->pstack.length == 0) {
		return (AST_Node){}; // shouldn't happen. lol, famous last words
	}
	return pctx->pstack.data[pctx->pstack.top];
}

AST_Node pctx_peek_offset(parse_ctx* pctx, int n) {
	if (pctx->pstack.length <= n) {
		return (AST_Node){.nodeType=AST_NODE_TYPE_UNDEFINED}; // shouldn't happen. lol, famous last words
	}
	return pctx->pstack.data[pctx->pstack.top - n];
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
		// printf("%d\n", i);
		ast_print_node(pctx->pstack.data[i], 0);
	}
}

void pctx_print_stack_lite(parse_ctx* pctx) {
	for (int i = 0; i < pctx->pstack.length; i++) {
		AST_Node n =pctx->pstack.data[i]; 
		ast_print_node_lite(n);
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
			// printf("DoubleLit\n");
			nt = AST_NODE_TYPE_TERMINAL;
			t = P_NEW_TERMINAL(TERMINAL_TYPE_DOUBLE_LIT, .dbl_lit=interpret_double_sv_to_double(tok.text));
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
		default: break;
	}
	*out_n = (AST_Node) {.nodeType=nt, .terminal=t, .state=tok.state};
	return status;
}

int try_convert_token_to_operator(token tok, AST_Node* out_n) {
	AST_NodeType nt = AST_NODE_TYPE_UNDEFINED;
	Operator op;
	int status = 0; // 0 indicates no reduction
	switch (tok.type) {
		case T_LOGIC_BEG...T_LOGIC_END:
			nt = AST_NODE_TYPE_OPERATOR;
			op=P_NEW_OPERATOR(OPERATOR_TYPE_LOGIC, .op=tok.text);
			status = 1;
			break;
		case T_STACK_BEG...T_STACK_END:
			nt = AST_NODE_TYPE_OPERATOR;
			op=P_NEW_OPERATOR(OPERATOR_TYPE_STACK, .op=tok.text);
			status = 1;
			break;
		case T_ARITH_BEG...T_ARITH_END:
			nt = AST_NODE_TYPE_OPERATOR;
			op=P_NEW_OPERATOR(OPERATOR_TYPE_ARITH, .op=tok.text);
			status = 1;
			break;
		default: break;
	}
	*out_n = (AST_Node) {.nodeType=nt, .op = op, .state=tok.state};
	return status;
}

int try_convert_token_to_reserved(token tok, AST_Node* out_n) {
	AST_NodeType nt = AST_NODE_TYPE_UNDEFINED;
	Reserved res;
	int status = 0; // 0 indicates no reduction
	switch (tok.type) {
		case T_RESERVE_BEG...T_RESERVE_END:
			nt = AST_NODE_TYPE_RESERVED;
			res=P_NEW_RESERVED(tok);
			status = 1;
			break;
		default: break;
	}
	*out_n = (AST_Node) {.nodeType = nt, .reserved = res};
	return status;
}

// See ast.h for grammar reference
int try_reduce(parse_ctx* pctx, AST_Node* out_n) {
	*out_n = (AST_Node){0};

	// term -> expression
	if (pctx_peek_offset(pctx, 0).nodeType == AST_NODE_TYPE_TERM) {
		AST_Node n = pctx_peek(pctx);
		out_n->nodeType = AST_NODE_TYPE_STATEMENT_EXPRESSION;
		out_n->stmtExpr.expr = calloc(1, sizeof(Expression));
		out_n->stmtExpr.expr->type = EXPRESSION_TYPE_TERM;
		out_n->stmtExpr.expr->ETerm.term = n.term;
		out_n->stmtExpr.expr->state = n.state;
		return 1;
	}

	// expression expression op -> expression
	if (pctx_peek_offset(pctx, 2).nodeType == AST_NODE_TYPE_STATEMENT_EXPRESSION &&
			pctx_peek_offset(pctx, 1).nodeType == AST_NODE_TYPE_STATEMENT_EXPRESSION &&
			pctx_peek_offset(pctx, 0).nodeType == AST_NODE_TYPE_OPERATOR)
	{
		AST_Node expr1 = pctx_peek_offset(pctx, 2);
		AST_Node expr2 = pctx_peek_offset(pctx, 1);
		AST_Node operator = pctx_peek_offset(pctx, 0);
		out_n->nodeType = AST_NODE_TYPE_STATEMENT_EXPRESSION;
		out_n->stmtExpr.type = STATEMENT_EXPR_TYPE_EXPRESSION;
		out_n->stmtExpr.expr = malloc(sizeof(Expression));
		out_n->stmtExpr.expr->type = EXPRESSION_TYPE_EEO;
		out_n->stmtExpr.expr->EEO.left = expr1.stmtExpr.expr;
		out_n->stmtExpr.expr->EEO.right = expr2.stmtExpr.expr;
		out_n->stmtExpr.expr->EEO.operation = operator.op;
		out_n->stmtExpr.expr->state = expr1.state;
		return 3;
	}

	// expression id -> procedure_call 
	if (pctx_peek_offset(pctx, 1).nodeType == AST_NODE_TYPE_STATEMENT_EXPRESSION &&
			pctx_peek_offset(pctx, 1).stmtExpr.type == STATEMENT_EXPR_TYPE_EXPRESSION &&
			pctx_peek_offset(pctx, 0).nodeType == AST_NODE_TYPE_TERMINAL &&
			pctx_peek_offset(pctx, 0).terminal.type == TERMINAL_TYPE_IDENTIFIER) {
		AST_Node expr = pctx_peek_offset(pctx, 1);
		AST_Node id = pctx_peek_offset(pctx, 0);
		out_n->nodeType = AST_NODE_TYPE_STATEMENT_EXPRESSION;
		out_n->stmtExpr.type = STATEMENT_EXPR_TYPE_EXPRESSION;
		out_n->stmtExpr.expr = malloc(sizeof(Expression));
		out_n->stmtExpr.expr->type = EXPRESSION_TYPE_PROC_CALL;
		out_n->stmtExpr.expr->EProcCall.proc_call.name = id.terminal.id;
		out_n->stmtExpr.expr->state = expr.state;
		return 1;
	}

	// reduce block (simple block for now)
	// <block>       := '{' <expressions> '}'
	//  expressions is just a sequence of expressions
	if (pctx_peek_offset(pctx, 0).nodeType == AST_NODE_TYPE_RESERVED &&
			pctx_peek_offset(pctx, 0).reserved.token.type == T_RBRC) {
		bool is_valid_block = true;
		const char* msg = "";
		AST_Node ref;
		int offset;
		for (offset = 1; ; offset++) {
			// Have we searched past the extent of the stack? 
			// If so we wont find the opening '{' and thus it is missing
			//    Fail case
			if (pctx->pstack.top - offset < 0) {
				is_valid_block = false;
				msg = "No opening '{' found";
				break;
			}
			// Have we encountered the original opening '{' token? (Remember we are searching backwards) 
			//    Success case
			//    nodes from 1..offset-1 should go into block ast
			if (pctx_peek_offset(pctx, offset).nodeType == AST_NODE_TYPE_RESERVED &&
					pctx_peek_offset(pctx, offset).reserved.token.type == T_LBRC) {
				break;
			}
			// Are the contents of the block valid? Type StatementExpression?
			//    Fail case
			if (pctx_peek_offset(pctx, offset).nodeType != AST_NODE_TYPE_STATEMENT_EXPRESSION) {
				is_valid_block = false;
				msg = "Only expressions and statements are allowed within a block.";
				ref = pctx_peek_offset(pctx, offset);
				break;
			}
		}
		// If the loop above failed, report the failure and exit the program
		if (!is_valid_block) {
			sl_assert(0, "Error while parsing block\n\t%s\n\twith nodeType: %d\n", msg, ref.nodeType == AST_NODE_TYPE_UNDEFINED ? -1 : ref.nodeType);
		}

		// If the loop above didn't fail, we have a valid code block
		out_n->nodeType = AST_NODE_TYPE_BLOCK;
		if (offset == 1) {
			return 2;
		}
		// subtract 1 because we dont want to count the expression for the if condition
		// printf("Number of expressions in block: %d\n", offset - 1); 
		sl_log("offset: %d\n", offset);
		// Put the expressions and statements into the block node
		for (int i = 0; i < offset - 1; i++) {
			AST_Node n = pctx_peek_offset(pctx, offset - 1 - i);
			cvector_push_back(out_n->block.items, n.stmtExpr);
		}

		pctx_print_stack_lite(pctx);
		// Return with the number of nodes to pop (the inner expressions + the '{' and '}')
		return offset + 2;
	}

	if (pctx_peek_offset(pctx, 0).nodeType == AST_NODE_TYPE_RESERVED &&
			pctx_peek_offset(pctx, 0).reserved.token.type == T_IF) {
		sl_log("Found <if>");
	}
	if (pctx_peek_offset(pctx, 0).nodeType == AST_NODE_TYPE_BLOCK) {
		sl_log("Found <block>");
	}

	// 'if' expression block -> if
	//  NOTE: Incomplete
	if (pctx_peek_offset(pctx, 2).nodeType == AST_NODE_TYPE_RESERVED &&
			pctx_peek_offset(pctx, 2).reserved.token.type == T_IF &&
			pctx_peek_offset(pctx, 1).nodeType == AST_NODE_TYPE_STATEMENT_EXPRESSION &&
			pctx_peek_offset(pctx, 1).stmtExpr.type == STATEMENT_EXPR_TYPE_EXPRESSION &&
			pctx_peek_offset(pctx, 0).nodeType == AST_NODE_TYPE_BLOCK) {
		sl_assert(0, "Reducing if statements unsupported right now");
	}

	// reduce terminals
	if (pctx_peek_offset(pctx, 0).nodeType == AST_NODE_TYPE_TERMINAL) {
		AST_Node n = pctx_peek(pctx);
		switch (n.terminal.type) {
			case TERMINAL_TYPE_IDENTIFIER:
				return 0;
			case TERMINAL_TYPE_HEX_LIT:
				out_n->nodeType = AST_NODE_TYPE_TERM;
				out_n->term.type = TERM_TYPE_HEX_LIT;
				out_n->term._integer = n.term._integer;
				out_n->term.state = n.state;
				return 1;
			case TERMINAL_TYPE_DOUBLE_LIT:
				out_n->nodeType = AST_NODE_TYPE_TERM;
				out_n->term.type = TERM_TYPE_DOUBLE_LIT;
				out_n->term._double = n.term._double;
				out_n->term.state = n.state;
				return 1;
			case TERMINAL_TYPE_DEC_LIT:
				out_n->nodeType = AST_NODE_TYPE_TERM;
				out_n->term.type = TERM_TYPE_DEC_LIT;
				out_n->term._integer = n.term._integer;
				out_n->term.state = n.state;
				return 1;
			case TERMINAL_TYPE_STRING_LIT:
				out_n->nodeType = AST_NODE_TYPE_TERM;
				out_n->term.type = TERM_TYPE_STRING_LIT;
				out_n->term._string = n.term._string;
				out_n->term.state = n.state;
				return 1;
			case TERMINAL_TYPE_CHAR_LIT:
				out_n->nodeType = AST_NODE_TYPE_TERM;
				out_n->term.type = TERM_TYPE_CHR_LIT;
				out_n->term._chr = n.term._chr;
				out_n->term.state = n.state;
				return 1;
			// case TERMINAL:
			// 	out_n->nodeType = AST_NODE_TYPE_OPERATOR;
			// 	out_n->op.type = OPERATOR_TYPE_LOGIC;
			// 	out_n->op = n.terminal.operatorr;
			// 	out_n->op.state = n.state;
			// 	return 1;
			// case TERMINAL_TYPE_ARITH_OP:
			// 	out_n->nodeType = AST_NODE_TYPE_OPERATOR;
			// 	out_n->op.type = OPERATOR_TYPE_ARITH;
			// 	out_n->op = n.terminal.operatorr;
			// 	out_n->op.state = n.state;
			// 	return 1;
			// case TERMINAL_TYPE_STACK_OP:
			// 	assert(0 && "Stack op not implemented properly");
			// 	out_n->nodeType = AST_NODE_TYPE_EXPRESSION;
			// 	out_n->expression = calloc(1, sizeof(Expression));
			// 	out_n->expression->type = EXPRESSION_TYPE_STACK_OP;
			// 	out_n->expression->EEO.operation = n.terminal.operatorr;
			// 	out_n->expression->state = n.state;
			// 	return 1;
		}
	}

	return 0; // didn't reduce anything
}

int ishexchar(char c) {
	c = tolower(c);
	return c >= 'a' && c <= 'f';
}

int interpret_decimal_sv_to_int(String_View sv) {
	int v = 0;
	for (int i = 0; i < sv.count; i++) {
		int exponent = (sv.count - i - 1);
		int digit = sv.data[i] - '0';
		v += (int)pow(10, exponent) * digit;
	}
	return v;
}

int interpret_hex_sv_to_int(String_View sv) {
	sv_chop_left(&sv, 2);   // chop off the "0x"
	int value = 0;
	for (int i = 0; i < sv.count; i++) {
		int digit = 0;
		if (isdigit(sv.data[i])) {
			digit = sv.data[i] - '0';
		}
		else if (ishexchar(sv.data[i])) {
			digit = sv.data[i] - 'a' + 10;
		}
		int exponent = (sv.count - i - 1);
		int v = (int)pow(16, exponent) * digit;
		value += (int)pow(16, exponent) * digit;
	}
	return value;
}

// 42542.423
double interpret_double_sv_to_double(String_View sv) {
	String_View left = sv_chop_by_delim(&sv, '.');
	int integer_part = interpret_decimal_sv_to_int(left);
	int decimal_part = interpret_decimal_sv_to_int(sv);
	double decimal = (double) decimal_part / pow(10, sv.count);
	return integer_part + decimal;
}
