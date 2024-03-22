#include "interpreter.h"
#include "sv.h"
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

/***
 *  For interpreting the code, I think building a parse tree is worth while
 *  Making use of a symbol table in an interesting way could be handy for implementing function calls later on as well
 */

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
