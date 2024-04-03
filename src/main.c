#include "ast.h"
#include "ast_helper.h"
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

		AST_Node n;
		int p;
		// 1. Convert the token in the 
		if ((p = try_convert_token_to_terminal(tok, &n)) != 0) {
			pctx_push(&pctx, n);
		}
		else {
			fprintf(stderr, "Couldn't convert the token, [" SV_Fmt "] to a terminal. Continuing past it anyways.\n", SV_Arg(tok.text));
			continue;
		}

		while ((p = try_reduce(&pctx, &n)) != 0) {
			pctx_pop_n(&pctx, p);
			pctx_push(&pctx, n);
		}
	}

	for (int i = 0; i <= pctx.pstack.top; i++) {
		cvector_push_back(program.program.p, pctx.pstack.data[i]);
	}
	if (ai.ptree_given) {
		ast_print_program(program.program, 0);
	}

	interpreter_ctx ictx = ictx_new();
	ictx_run(&ictx, program.program);

	tctx_free(&ctx);

	// I think this causes a crash?
	// cmdline_parser_free(&ai);
}
