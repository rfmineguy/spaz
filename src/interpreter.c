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

interpreter_ctx ictx_new() {
	interpreter_ctx ctx = {0};
	return ctx;
}

void ictx_run(interpreter_ctx* ictx, Program p) {
	
}
