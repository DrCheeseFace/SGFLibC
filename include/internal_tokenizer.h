#ifndef INTERNAL_TOKENIZER_H
#define INTERNAL_TOKENIZER_H

#include <mr_utils.h>

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

// destroy using SGF_internal_tokens_destroy()
MrvVector *SGF_internal_tokeize(const char *input);
void SGF_internal_tokens_destroy(MrvVector *tokens);

#endif
