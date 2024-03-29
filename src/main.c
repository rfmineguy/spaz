#include "ast.h"
#include "ast_helper.h"
#include "interpreter.h"
#include "tokenizer.h"
#include "parser.h"
#include <stdio.h>
#include <assert.h>

int main() {
	tokenizer_ctx ctx = tctx_from_file("main.lang");
	parse_ctx ictx = pctx_new(100);

	token tok;
	while ((tok = tctx_get_next(&ctx)).type != T_EOF) {
		tctx_advance(&ctx);

		// Attempt to CONVERT the current token to a Terminal ast node
		//   No reduction here yet
		AST_Node n;
		if (try_convert_token_to_terminal(tok, &n) == 1) {
			// printf("Converted token to terminal node.\n");
			ast_print_node(n, 0);
			pctx_push(&ictx, n);
		}

		while (1) {
			AST_Node n;
			int pop_count;
			if ((pop_count = try_reduce(&ictx, &n)) != 0) {
				pctx_pop_n(&ictx, pop_count);
				pctx_push(&ictx, n);
				continue;
			}
			break;
		}

		// pctx_print_stack(&ictx);

		// "Reduce" terminals
		// AST_Node node;
		// Terminal t = {0};
		// AST_NodeType nt = AST_NODE_TYPE_UNDEFINED;

		/*
		switch (tok.type) {
			case T_ID: 
				nt = AST_NODE_TYPE_TERMINAL;
				t = P_NEW_TERMINAL(TERMINAL_TYPE_IDENTIFIER, .id=tok.text);
				break;
			case T_HEX_LIT:
				nt = AST_NODE_TYPE_TERMINAL;
				t = P_NEW_TERMINAL(TERMINAL_TYPE_HEX_LIT, .integer_lit=interpret_hex_sv_to_int(tok.text));
				break;
			case T_DOUBLE_LIT:
				nt = AST_NODE_TYPE_TERMINAL;
				t=P_NEW_TERMINAL(TERMINAL_TYPE_DOUBLE_LIT, .dbl_lit=interpret_double_sv_to_double(tok.text));
				break;
			case T_DECIMAL_LIT:
				nt = AST_NODE_TYPE_TERMINAL;
				t=P_NEW_TERMINAL(TERMINAL_TYPE_DEC_LIT, .integer_lit=interpret_decimal_sv_to_int(tok.text));
				break;
			case T_STRING_LIT:
				nt = AST_NODE_TYPE_TERMINAL;
				t=P_NEW_TERMINAL(TERMINAL_TYPE_STRING_LIT, .str_lit=tok.text);
				break;
			case T_CHAR_LIT:
				nt = AST_NODE_TYPE_TERMINAL;
				t=P_NEW_TERMINAL(TERMINAL_TYPE_CHAR_LIT, .chr_lit=tok.text);
				break;
			case T_LOGIC_BEG...T_LOGIC_END:
				nt = AST_NODE_TYPE_TERMINAL;
				t=P_NEW_TERMINAL(TERMINAL_TYPE_LOGIC_OP, .operatorr=tok.text.data[0]);
				break;
			case T_STACK_BEG...T_STACK_END:
				nt = AST_NODE_TYPE_TERMINAL;
				t=P_NEW_TERMINAL(TERMINAL_TYPE_STACK_OP, .operatorr=tok.text.data[0]);
				break;
			case T_ARITH_BEG...T_ARITH_END:
				nt = AST_NODE_TYPE_TERMINAL;
				t=P_NEW_TERMINAL(TERMINAL_TYPE_ARITH_OP, .operatorr=tok.text.data[0]);
				break;
			case T_RESERVE_BEG...T_RESERVE_END:
				nt = AST_NODE_TYPE_TERMINAL;
				t=P_NEW_TERMINAL(TERMINAL_TYPE_RESERVED, .reserved=((Reserved) {.type=tok.type, .str=tok.text}));
				break;
			default: break;
		}
		*/

		// node = (AST_Node) {.nodeType=nt, .terminal=t};
		// pctx_push(&ictx, node);

		// Start the reduce loop
		/*
		while (1) {
			if (pctx_top_is(&ictx, NT_LIST(AST_NODE_TYPE_TERMINAL), 1)) {
				AST_Node n = pctx_peek(&ictx);
				// term := <declit> | <hexlit> | <dbllit> | <strlit> | <chrlit>
				switch (n.terminal.type) {
					case TERMINAL_TYPE_DEC_LIT:    
						n.nodeType = AST_NODE_TYPE_TERM;
						n.term.type = TERM_TYPE_DEC_LIT;
						n.term._integer = n.terminal.integer_lit;
						break;
					case TERMINAL_TYPE_HEX_LIT:
						n.nodeType = AST_NODE_TYPE_TERM;
						n.term.type = TERM_TYPE_HEX_LIT;
						n.term._integer = n.terminal.integer_lit;
						break;
					case TERMINAL_TYPE_DOUBLE_LIT:
						n.nodeType = AST_NODE_TYPE_TERM;
						n.term.type = TERM_TYPE_DOUBLE_LIT;
						n.term._double = n.terminal.dbl_lit;
						break;
					case TERMINAL_TYPE_STRING_LIT:
						n.nodeType = AST_NODE_TYPE_TERM;
						n.term.type = TERM_TYPE_STRING_LIT;
						n.term._string = n.terminal.str_lit;
						break;
					case TERMINAL_TYPE_CHAR_LIT:
						n.nodeType = AST_NODE_TYPE_TERM;
						n.term.type = TERM_TYPE_CHR_LIT;
						n.term._chr = n.terminal.chr_lit;
						break;
					default: break;
				}
				// operator := <arith_op> | <logic_op>
				switch (n.terminal.type) {
					case TERMINAL_TYPE_ARITH_OP:
						n.nodeType = AST_NODE_TYPE_OPERATOR;
						n.op.type = OPERATOR_TYPE_ARITH;
						n.op.op = n.terminal.operatorr;
						break;
					case TERMINAL_TYPE_LOGIC_OP:
						n.op.type = OPERATOR_TYPE_LOGIC;
						n.nodeType = AST_NODE_TYPE_OPERATOR;
						n.op.op = n.terminal.operatorr;
						break;
					default: break;
				}
				pctx_pop(&ictx);
				pctx_push(&ictx, n);
				pctx_print_stack(&ictx);
			}
		}
		*/
	}

	tctx_free(&ctx);
}
