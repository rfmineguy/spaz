#ifndef CONVERT_H
#define CONVERT_H
#include "sv.h"

int    convert_decimal_sv_to_int(String_View);
int    convert_hex_sv_to_int(String_View);
double convert_double_sv_to_double(String_View);

bool   convert_is_sv_int(String_View);
bool   convert_is_sv_hex(String_View);
bool   convert_is_sv_dec(String_View);
bool   convert_is_sv_double(String_View);

#endif
