#include "tokenizer.h"
#include "parser.h"
#include <stdio.h>

int main() {
	tokenizer_ctx ctx = tctx_from_file("main.lang");
	parse_ctx ictx = pctx_new(100);

	token tok;
	while ((tok = tctx_get_next(&ctx)).type != T_EOF) {
		tctx_advance(&ctx);
		// printf("%11s -> \"" SV_Fmt "\"\n", token_str(tok.type), SV_Arg(tok.text));
		pctx_push(&ictx, PSNODE_NEW_TERMINAL(tok)); // push the token onto the stack

		// 1. Check if the top of the stack is reducable
		//   1a. Yes? Reduce the stack using the given rule
		//   1b. No?  Push the next token onto the stack
		// 2. Repeat 1 until the entire file is parsed
		if (pctx_top_is(&ictx, NT_LIST(PSNT_TERMINAL), 1)) {
			parse_stack_node t = pctx_peek(&ictx);
			printf("%s\n", pctx_type_str(t.type));
			// pctx_push(&ictx, (parse_stack_node) {});
			continue;
		}
	}

	tctx_free(&ctx);
}
