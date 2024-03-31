#include "ast.h"
#include "ast_helper.h"
#include "cvector.h"
#include "interpreter.h"
#include "tokenizer.h"
#include "parser.h"
#include <stdio.h>
#include <assert.h>

int main() {
	tokenizer_ctx ctx = tctx_from_file("shorter.lang");
	parse_ctx pctx = pctx_new(100);
	AST_Node program = (AST_Node) {.nodeType=AST_NODE_TYPE_PROGRAM};

	token tok;
	while ((tok = tctx_get_next(&ctx)).type != T_EOF) {
		tctx_advance(&ctx);

		AST_Node n;
		int p;
		if ((p = try_convert_token_to_terminal(tok, &n)) != 0) {
			pctx_push(&pctx, n);
		}

		while ((p = try_reduce(&pctx, &n)) != 0) {
			pctx_pop_n(&pctx, p);
			pctx_push(&pctx, n);
		}
	}

	for (int i = 0; i <= pctx.pstack.top; i++) {
		cvector_push_back(program.program.p, pctx.pstack.data[i]);
	}
	//ast_print_program(program.program, 0);

	interpreter_ctx ictx = ictx_new();
	ictx_run(&ictx, program.program);

	tctx_free(&ctx);
}
