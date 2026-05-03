#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <mr_utils.h>

typedef enum {
	TOKEN_PAREN_OPEN,
	TOKEN_PAREN_CLOSE,
	TOKEN_SEMICOLON,
	TOKEN_PROPERTY,
	TOKEN_VALUE,
	TOKEN_EOF,
	TOKEN_ERROR
} TokenType;

typedef struct {
	TokenType type;
	char *text;
} Token;

Token create_token(TokenType type, const char *start, int length);

void destroy_tokens(MrvVector *tokens);

MrvVector *tokenize(const char *input);


#endif
