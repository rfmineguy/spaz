#define DEBUG_AST 1
#include "ast.h"
#include "ast_free.h"
#include "ast_print.h"
#include "cvector.h"
#include "interpreter.h"
#include "tokenizer.h"
#include "parser.h"
#include <stdio.h>
#include "../gengetopt/cmdline.h"

int main(int argc, char** argv) {
	struct gengetopt_args_info ai;
	if (cmdline_parser(argc, argv, &ai) != 0) {
		fprintf(stderr, "Failed to parse arguments\n");
		return 1;
	}

	if (!ai.file_given) {
		fprintf(stderr, "No file specified\n");
		return 2;
	}

	tokenizer_ctx ctx = tctx_from_file(ai.file_arg);
	parse_ctx pctx = pctx_new(100);
	AST_Node program = (AST_Node) {.nodeType=AST_NODE_TYPE_PROGRAM};

	token tok;
	while ((tok = tctx_get_next(&ctx)).type != T_EOF) {
		tctx_advance(&ctx);
		if (tok.type == T_EOF) break;

		AST_Node n;
		int p;
		if ((p = try_convert_token_to_terminal(tok, &n)) != 0) {
			pctx_push(&pctx, n);
		}
		else if ((p = try_convert_token_to_operator(tok, &n)) != 0) {
			pctx_push(&pctx, n);
		}
		else if ((p = try_convert_token_to_reserved(tok, &n)) != 0) {
			pctx_push(&pctx, n);
		}
		else {
			if (tok.type == T_EOF) {
				break;
			}
			fprintf(stderr, "Couldn't convert the token, [str=" SV_Fmt ", v=%d] to a terminal."
											"Continuing past it anyways.\n",
											SV_Arg(tok.text), tok.type);
			fprintf(stderr, "%*s\n", 4, ctx.state.cursor);
			continue;
		}

		int reduceCount = 0;
		while ((p = try_reduce(&pctx, &n)) != 0) {
			reduceCount++;
			pctx_pop_n(&pctx, p);
			pctx_push(&pctx, n);
		}
	}
	if (ai.verbose_given) {
		printf("Printing parse stack\n");
		printf("==========================================\n");
		pctx_print_stack(&pctx);
		printf("==========================================\n");
	}

	for (int i = 0; i <= pctx.pstack.top; i++) {
		cvector_push_back(program.program.p, pctx.pstack.data[i]);
	}
	if (ai.ptree_given) {
		printf("Printing program\n");
		printf("==========================================\n");
		ast_print_node(program, 0);
		printf("==========================================\n");
	}

	if (ai.interpret_given) {
		interpreter_ctx ictx = ictx_new();
		ictx_run(&ictx, program.program);
	}

	ast_free_program(program.program);
	tctx_free(&ctx);
	pctx_free(&pctx);

	// I think this causes a crash?
	cmdline_parser_free(&ai);
}
