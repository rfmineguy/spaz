#include "interpreter_builtins.h"
#include "interpreter.h"
#include "convert.h"
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
		case UNDEFINED:
			printf("type_value=%d, type=%s", sn.type, ictx_stack_node_type_to_str(UNDEFINED));
			break;
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
}
