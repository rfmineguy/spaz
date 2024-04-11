#include "convert.h"
#include <math.h>

int ishexchar(char c) {
	c = tolower(c);
	return c >= 'a' && c <= 'f';
}

int convert_decimal_sv_to_int(String_View sv) {
	int v = 0;
	for (int i = 0; i < sv.count; i++) {
		int exponent = (sv.count - i - 1);
		int digit = sv.data[i] - '0';
		v += (int)pow(10, exponent) * digit;
	}
	return v;
}

int convert_hex_sv_to_int(String_View sv) {
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
double convert_double_sv_to_double(String_View sv) {
	String_View left = sv_chop_by_delim(&sv, '.');
	int integer_part = convert_decimal_sv_to_int(left);
	int decimal_part = convert_decimal_sv_to_int(sv);
	double decimal = (double) decimal_part / pow(10, sv.count);
	return integer_part + decimal;
}

bool convert_is_sv_int(String_View sv) {
	return convert_is_sv_hex(sv) || convert_is_sv_int(sv);
}

bool convert_is_sv_hex(String_View sv) {
	for (int i = 0; i < sv.count; i++) {
		if (!ishexchar(sv.data[i])) {
			return false;
		}
	}
	return true;
}

bool convert_is_sv_dec(String_View sv) {
	for (int i = 0; i < sv.count; i++) {
		if (!isdigit(sv.data[i])) {
			return false;
		}
	}
	return true;
}
bool convert_is_sv_double(String_View sv) {
	int decimalpointcount = 0;
	for (int i = 0; i < sv.count; i++) {
		if (sv.data[i] == '.') {
			decimalpointcount++;
			continue;
		}
		if (isdigit(sv.data[i]) == 0) {
			// if we find any non digit characters, its not a double
			return false;
		}
	}
	return decimalpointcount == 1;
}
