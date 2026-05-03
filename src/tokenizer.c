#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <internal_tokenizer.h>
#include <mr_utils.h>

mr_internal void token_init(SGF_Token *token, SGF_TokenType type,
			    const char *start, size_t length)
{
	token->type = type;
	token->text = malloc(length + 1);
	if (token->text) {
		strncpy(token->text, start, length);
		token->text[length] = '\0';
	}
}

MrvVector *SGF_internal_tokeize(const char *input)
{
	if (input == NULL)
		return NULL;

	MrvVector *tokens = mrv_create(32, sizeof(SGF_Token));
	const char *p = input;

	while (*p != '\0') {
		if (isspace(*p)) {
			p++;
			continue;
		}

		SGF_Token t;

		if (*p == '(') {
			token_init(&t, SGF_TOKEN_PAREN_OPEN, p++, 1);
		} else if (*p == ')') {
			token_init(&t, SGF_TOKEN_PAREN_CLOSE, p++, 1);
		} else if (*p == ';') {
			token_init(&t, SGF_TOKEN_SEMICOLON, p++, 1);
		} else if (isupper(*p)) {
			const char *start = p;
			while (isupper(*p))
				p++;
			token_init(&t, SGF_TOKEN_PROPERTY, start, p - start);
		} else if (*p == '[') {
			p++; // skip '['
			const char *start = p;
			while (*p != ']' && *p != '\0') {
				if (*p == '\\' && *(p + 1) != '\0')
					p++;
				p++;
			}
			token_init(&t, SGF_TOKEN_VALUE, start, p - start);
			if (*p == ']')
				p++;
		} else {
			token_init(&t, SGF_TOKEN_ERROR, p++, 1);
		}

		mrv_append(tokens, &t, APPEND_SCALING_DOUBLE);
	}

	return tokens;
}

void SGF_internal_tokens_destroy(MrvVector *tokens)
{
	if (tokens == NULL)
		return;

	for (size_t i = 0; i < tokens->len; i++) {
		SGF_Token *token = mrv_get_idx(tokens, i);
		if (token->text) {
			free(token->text);
		}
	}

	mrv_destroy(tokens);
}
