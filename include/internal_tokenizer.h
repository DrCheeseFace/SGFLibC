#ifndef INTERNAL_TOKENIZER_H
#define INTERNAL_TOKENIZER_H

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

void token_init(Token *token, TokenType type, const char *start, int length);

MrvVector *tokenize(const char *input);
// destory for tokenize()
void tokens_destroy(MrvVector *tokens);


#endif
