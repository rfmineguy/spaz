#include "interpreter_builtins.h"
#include "interpreter.h"
#include "convert.h"
#include "sl_assert.h"
#include <stdio.h>

void interp_builtin_print(stack_node sn) {
	switch (sn.type) {
		case INTEGER:
			printf("%d", sn.integerLiteral);
			break;
		case CHAR:
			printf(SV_Fmt, SV_Arg(sn.charLiteral));
			break;
		case STRING: {
			String_View s;
			s.data = sn.stringLiteral.data + 1;
			s.count = sn.stringLiteral.count - 2;
			printf(SV_Fmt, SV_Arg(s));
			break;
		}
		case DOUBLE:
			printf("%0.4f", sn.doubleLiteral);
			break;
		case STACK_OP: sl_assert(0, "Printing stackop not supported yet"); break;
		case UNDEFINED:
			printf("type_value=%d, type=%s", sn.type, ictx_stack_node_type_to_str(UNDEFINED));
			break;
		default: printf("print failed. sn.type = %d\n", sn.type);
	}
}

void interp_builtin_println(stack_node sn) {
	interp_builtin_print(sn);
	printf("\n");
} 

void interp_builtin_input(stack_node* o_sn) {
	// Should this function should be generic.
	//   - what should happen when the user inptut is an integer,double,string, etc
	static char buf[255];
	fgets(buf, 255, stdin);
	buf[strcspn(buf, "\n")] = 0; // remove newline
	String_View input = sv_from_cstr(buf);
	if (convert_is_sv_dec(input)) {
		int decimal = convert_decimal_sv_to_int(input);
		o_sn->type = INTEGER;
		o_sn->integerLiteral = decimal;
		return;
	}
	if (convert_is_sv_hex(input)) {
		int hex = convert_decimal_sv_to_int(input);
		o_sn->type = INTEGER;
		o_sn->integerLiteral = hex;
		return;
	}
	if (convert_is_sv_double(input)) {
		double dbl = convert_double_sv_to_double(input);
		o_sn->type = DOUBLE;
		o_sn->doubleLiteral = dbl;
		return;
	}
	o_sn->type = STRING;
	o_sn->stringLiteral = input;
}

void interp_builtin_showstack(interpreter_ctx* ictx) {
	for (int i = ictx->stack_top; i >= 0; i--) {
		stack_node n = ictx->stack[i];
		printf("%d: ", i);
		switch (n.type) {
			case INTEGER:   printf("INTEGER: %d\n", n.integerLiteral); break;
			case DOUBLE:    printf("DOUBLE:  %0.4f\n", n.doubleLiteral); break;
			case STRING:    printf("STRING:  " SV_Fmt "\n", SV_Arg(n.stringLiteral)); break;
			case CHAR:      printf("CHAR:    " SV_Fmt "\n", SV_Arg(n.charLiteral)); break;
			case STACK_OP:  printf("STACKOP: " SV_Fmt "\n", SV_Arg(n.stackOp.op.op_str)); break;
			case UNDEFINED: printf("Undefined\n"); break;
		}
	}
}
