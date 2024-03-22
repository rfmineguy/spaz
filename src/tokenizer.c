#include "tokenizer.h"
#include <ctype.h>
#include <stdio.h>
#include <regex.h>

regex_t rnew(const char* r) {
	regex_t reg;
	if (regcomp(&reg, r, REG_EXTENDED) != 0) {
		printf("Failed to compile regex [%s]\n", r);
		exit(1);
	}
	return reg;
}

int rmatch(const char* s, const char* reg, int* length_out) {
	regex_t r = rnew(reg);
	regmatch_t match[1];
	if (regexec(&r, s, 1, match, 0) == 0) {
		// make sure the match was immediately at s
		if (match[0].rm_so != 0)
			return -1;
		const char* start = s + match[0].rm_so;
		const char* end = s + match[0].rm_eo;
		int len = end - start;
		*length_out = len;
		return 0;
	}
	regfree(&r);
	return -1;
}

char* read_file(const char* filename, size_t* length) {
	FILE* f = fopen(filename, "r");
	if (!f) {
		fprintf(stderr, "Failed to open file: %s\n", filename);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size_t l = ftell(f);
	fseek(f, 0, SEEK_SET);

	if (length) {
		*length = l;
	}
	char *content = calloc(l + 1, sizeof(char));
	size_t read_bytes = fread(content, sizeof(char), l, f);
	content[read_bytes] = 0;
	if (read_bytes != l) {
		fprintf(stderr, "Something went wrong reading: %lu != %lu\n", read_bytes, l);	
		free(content);
		content = NULL;
	}
	fclose(f);
	return content;
}

void tctx_internal_run_tokenizer(tokenizer_ctx* ctx) {
	
}

const char* token_str(token_type t) {
	switch (t) {
		case T_ID:      		return "ID";
		case T_FN:      		return "FN";
		case T_IF:      		return "IF";
		case T_ELSE: 				return "ELSE";
		case T_SWITCH:      return "SWITCH";
		case T_BREAK: 			return "BREAK";
		case T_DEFAULT: 		return "DEFAULT";
		case T_LP:      		return "LP";
		case T_RP:      		return "RP";
		case T_LBKT:				return "LKBT";
		case T_RBKT:				return "RKBT";
		case T_LBRC:				return "LBRC";
		case T_RBRC:				return "RBRC";
		case T_EQ: 					return "EQ"; 					 
		case T_GT: 					return "GT"; 
		case T_LT: 					return "LT";
		case T_GTEQ:				return "GTEQ"; 
		case T_LTEQ: 				return "LTEQ";
		case T_LOR:  				return "LOR";
		case T_LAND:   			return "LAND";
		case T_BOR:    			return "BOR";
		case T_BAND:   			return "BAND";
		case T_COLON:   		return "COLON";
		case T_COMMA: 			return "COMMA";
		case T_PERIOD: 			return "PERIOD";
		case T_MINUS:   		return "MINUS";
		case T_PLUS:    		return "PLUS";
		case T_MUL:     		return "MUL";
		case T_DIV:     		return "DIV";
		case T_MOD:  				return "MOD";
		case T_DOUBLE_LIT:  return "DOUBLE_LIT";
		case T_DECIMAL_LIT: return "DECIMAL_LIT";
		case T_HEX_LIT:   	return "HEX_LIT";
		case T_STRING_LIT:  return "STRING_LIT";
		case T_CHAR_LIT:    return "CHAR_LIT";
		case T_SQUOTE: 			return "SQUOTE";
		case T_DQUOTE:      return "DQUOTE";
		case T_ARROW:    		return "ARROW";
		case T_EOF: 				return "EOF";
		case T_SPACE:       return "SPACE";
		case T_UNKNOWN:  		return "UNKNOWN";
		default:            printf("%d\n", t); assert(0 && "Unimplemented token_str conversion");
	}
}

tokenizer_ctx tctx_from_file(const char* filename) {
	tokenizer_ctx ctx = {0};
	char* content = read_file(filename, &ctx.content_length);
	ctx.content = content;
	ctx.state.cursor = content;
	return ctx;
}

tokenizer_ctx tctx_from_cstr(const char* cstr) {
	tokenizer_ctx ctx = {0};
	ctx.content = strdup(cstr);
	ctx.content_length = strlen(cstr);
	ctx.state.cursor = ctx.content;
	return ctx;
}

void tctx_free(tokenizer_ctx* ctx) {
	free((void*) ctx->content);
}

#define RMATCH(str, t) \
	do {\
		int length;\
		if (rmatch(ctx->state.cursor, str, &length) != -1) {\
			const char* was = ctx->state.cursor;\
			/* ctx->state.cursor += length;*/\
			return (token) {.type = t, .text=(sv_from_parts(was, length))};\
		}\
	} while(0)

#define CHMATCH(c, t) \
	do {\
		int length;\
		if (*ctx->state.cursor == c) {\
			const char* was = ctx->state.cursor;\
			/* ctx->state.cursor += 1;*/ \
			return (token) {.type = t, .text=(sv_from_parts(was, 1))};\
		}\
	} while(0)

token tctx_advance(tokenizer_ctx* ctx) {
	token t = tctx_get_next(ctx);
	ctx->state.cursor += t.text.count;
	return t;
}

token tctx_get_next(tokenizer_ctx* ctx) {
	// Detect EOF
	if (ctx->state.cursor >= ctx->content + ctx->content_length - 1)
		return (token) {.type=T_EOF };

	// Consume spaces
	const char* start = ctx->state.cursor;
	char* cursor = (char*) ctx->state.cursor;
	while (isspace(*cursor) != 0) {
		cursor++;
	}
	// Also consume comments
	if (strncmp(cursor, "//", 2) == 0) {
		while (*cursor != '\n') {
			cursor++;
		}
		cursor++;
	}
	// Consume spaces again
	while (isspace(*cursor) != 0) {
		cursor++;
	}
	ctx->state.cursor = cursor;

	// Match code
	RMATCH("\\\"([^\\\"]|\n)*\\\"", T_STRING_LIT);
	RMATCH("\\\'(.)\\\'", T_CHAR_LIT);
	RMATCH("fn", T_FN);
	RMATCH("if", T_IF);
	RMATCH("else", T_ELSE);
	RMATCH("switch", T_SWITCH);
	RMATCH("break", T_BREAK);
	RMATCH("default", T_DEFAULT);
	RMATCH("->", T_ARROW);
	RMATCH("0x[0-9a-fA-F]+", T_HEX_LIT);
	RMATCH("[0-9]+\\.[0-9]+", T_DOUBLE_LIT);
	RMATCH("[0-9]+", T_DECIMAL_LIT);
	RMATCH("[a-zA-Z_][a-zA-Z0-9_]*", T_ID);
	RMATCH("\\|\\|", T_LOR);
	RMATCH("&&", T_LAND);
	CHMATCH('|', T_BOR);
	CHMATCH('&', T_BAND);
	RMATCH(">=", T_GTEQ);
	RMATCH("<=", T_LTEQ);
	CHMATCH('>', T_GT);
	CHMATCH('<', T_LT);
	CHMATCH(':', T_COLON);
	CHMATCH(',', T_COMMA);
	CHMATCH('.', T_PERIOD);
	CHMATCH('(', T_LP);
	CHMATCH(')', T_RP);
	CHMATCH('[', T_LBKT);
	CHMATCH(']', T_RBKT);
	CHMATCH('{', T_LBRC);
	CHMATCH('}', T_RBRC);
	CHMATCH('-', T_MINUS);
	CHMATCH('+', T_PLUS);
	CHMATCH('*', T_MUL);
	CHMATCH('/', T_DIV);
	CHMATCH('%', T_MOD);
	CHMATCH('=', T_EQ);
	CHMATCH('\'', T_SQUOTE);
	CHMATCH('\"', T_DQUOTE);

	return (token) {.type=T_UNKNOWN, .text=sv_from_parts(ctx->state.cursor, 1)};
}

void tctx_show_next_internal(tokenizer_ctx* ctx, int line) {
	token next = tctx_get_next(ctx);
	printf("[%d] Next: %s\n", line, token_str(next.type));
}

tokenizer_state tctx_save(tokenizer_ctx* ctx) {
	return ctx->state;
}

void tctx_restore(tokenizer_ctx* ctx, tokenizer_state state) {
	ctx->state = state;
}
