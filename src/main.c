#include "ast.h"
#include "ast_helper.h"
#include "interpreter.h"
#include "tokenizer.h"
#include "parser.h"
#include <stdio.h>
#include <assert.h>

int main() {
	tokenizer_ctx ctx = tctx_from_file("shorter.lang");
	parse_ctx ictx = pctx_new(100);

	token tok;
	while ((tok = tctx_get_next(&ctx)).type != T_EOF) {
		tctx_advance(&ctx);
		// printf("%s\n", token_str(tok.type));
		// if (tok.type == T_DOUBLE_LIT) {
		// 	printf("%0.4f\n", interpret_double_sv_to_double(tok.text));	
		// }

		// Attempt to CONVERT the current token to a Terminal ast node
		//   No reduction here yet
		AST_Node n;
		int p;
		if ((p = try_convert_token_to_terminal(tok, &n)) != 0) {
			pctx_push(&ictx, n);
		}
		// pctx_print_stack(&ictx);

		while ((p = try_reduce(&ictx, &n)) != 0) {
			// pctx_print_stack(&ictx);
			pctx_pop_n(&ictx, p);
			pctx_push(&ictx, n);
			// printf("Converted\n");
		}

		// printf("---------\n");
	}
	pctx_print_stack(&ictx);

	tctx_free(&ctx);
}
