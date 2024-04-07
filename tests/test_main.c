#include "munit/munit.h"
#include "../src/interpreter.h"
#include "../src/parser.h"
#include <stdio.h>

MunitResult decimal_sv_to_int     (const MunitParameter params[], void* fixture);
MunitResult hex_sv_to_int         (const MunitParameter params[], void* fixture);
MunitResult double_sv_to_double   (const MunitParameter params[], void* fixture);

MunitTest tests[] = {
	{"/decimal_sv_to_int",   		decimal_sv_to_int, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
	{"/hex_sv_to_int",       		hex_sv_to_int, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
	{"/double_sv_to_double", 		double_sv_to_double, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite suite = {
	"/interpret", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *const *argv) {
	return munit_suite_main(&suite, NULL, argc, argv);
}

MunitResult decimal_sv_to_int  (const MunitParameter params[], void* fixture) {
	int v = interpret_decimal_sv_to_int(SV("453"));
	munit_assert_int(v, ==, 453);
	v = interpret_decimal_sv_to_int(SV("293"));
	munit_assert_int(v, ==, 293);
	v = interpret_decimal_sv_to_int(SV("12345324"));
	munit_assert_int(v, ==, 12345324);
	v = interpret_decimal_sv_to_int(SV("9725"));
	munit_assert_int(v, ==, 9725);
	v = interpret_decimal_sv_to_int(SV("624517357"));
	munit_assert_int(v, ==, 624517357);
	v = interpret_decimal_sv_to_int(SV("8637645"));
	munit_assert_int(v, ==, 8637645);
	return MUNIT_OK;
}

MunitResult hex_sv_to_int  (const MunitParameter params[], void* fixture) {
	int v = interpret_hex_sv_to_int(SV("0x3a"));
	munit_assert_int(v, ==, 0x3a);
	v = interpret_hex_sv_to_int(SV("0x20"));
	munit_assert_int(v, ==, 0x20);
	v = interpret_hex_sv_to_int(SV("0x2ac8"));
	munit_assert_int(v, ==, 0x2ac8);
	v = interpret_hex_sv_to_int(SV("0x9c2ac8"));
	munit_assert_int(v, ==, 0x9c2ac8);
	v = interpret_hex_sv_to_int(SV("0x5c35aa"));
	munit_assert_int(v, ==, 0x5c35aa);
	v = interpret_hex_sv_to_int(SV("0x9c2ac8"));
	munit_assert_int(v, ==, 0x9c2ac8);
	v = interpret_hex_sv_to_int(SV("0x31942ff8"));
	munit_assert_int(v, ==, 0x31942ff8);
	return MUNIT_OK;
}

MunitResult double_sv_to_double(const MunitParameter params[], void* fixture) {
	double d = interpret_double_sv_to_double(SV("5.646247363"));
	munit_assert_double(d, ==, 5.646247363);
	d = interpret_double_sv_to_double(SV("5423.864213"));
	munit_assert_double(d, ==, 5423.864213);
	d = interpret_double_sv_to_double(SV("46843134.9753947"));
	munit_assert_double(d, ==, 46843134.9753947);
	d = interpret_double_sv_to_double(SV("4684134.9753947"));
	munit_assert_double(d, ==, 4684134.9753947);
	return MUNIT_OK;
}
