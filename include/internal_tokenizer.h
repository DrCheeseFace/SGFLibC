#ifndef INTERNAL_TOKENIZER_H
#define INTERNAL_TOKENIZER_H

#include <mr_utils.h>
#include <sgf.h>

typedef enum {
	SGF_TOKEN_PAREN_OPEN,
	SGF_TOKEN_PAREN_CLOSE,
	SGF_TOKEN_SEMICOLON,
	SGF_TOKEN_PROPERTY,
	SGF_TOKEN_VALUE,
	SGF_TOKEN_EOF,
	SGF_TOKEN_ERROR
} SGF_TokenType;

typedef struct {
	SGF_TokenType type;
	char *text;
} SGF_Token;

typedef MrvVector *SGF_Tokens;
// destroy using SGF_internal_tokens_destroy()
SGF_Tokens SGF_internal_tokeize(const char *input);
void SGF_internal_tokens_destroy(SGF_Tokens tokens);
void SGF_internal_token_init(SGF_Token *token, SGF_TokenType type,
			     const char *text, size_t text_len);

#endif
