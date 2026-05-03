#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <mr_utils.h>
#include <tokenizer.h>

Token create_token(TokenType type, const char *start, int length)
{
	Token t;
	t.type = type;
	t.text = malloc(length + 1);
	if (t.text) {
		strncpy(t.text, start, length);
		t.text[length] = '\0';
	}
	return t;
}

void destroy_tokens(MrvVector *tokens)
{
	if (tokens == NULL)
		return;

	for (size_t i = 0; i < tokens->len; i++) {
		Token *token = mrv_get_idx(tokens, i);
		if (token->text) {
			free(token->text);
		}
	}

	mrv_destroy(tokens);
}

MrvVector *tokenize(const char *input)
{
	if (input == NULL)
		return NULL;

	MrvVector *tokens = mrv_create(32, sizeof(Token));
	const char *p = input;

	while (*p != '\0') {
		if (isspace(*p)) {
			p++;
			continue;
		}

		Token t;

		if (*p == '(') {
			t = create_token(TOKEN_PAREN_OPEN, p++, 1);
		} else if (*p == ')') {
			t = create_token(TOKEN_PAREN_CLOSE, p++, 1);
		} else if (*p == ';') {
			t = create_token(TOKEN_SEMICOLON, p++, 1);
		} else if (isupper(*p)) {
			const char *start = p;
			while (isupper(*p))
				p++;
			t = create_token(TOKEN_PROPERTY, start,
					 (int)(p - start));
		} else if (*p == '[') {
			p++; // skip '['
			const char *start = p;
			while (*p != ']' && *p != '\0') {
				if (*p == '\\' && *(p + 1) != '\0')
					p++;
				p++;
			}
			t = create_token(TOKEN_VALUE, start, (int)(p - start));
			if (*p == ']')
				p++;
		} else {
			t = create_token(TOKEN_ERROR, p++, 1);
		}

		mrv_append(tokens, &t, APPEND_SCALING_DOUBLE);
	}

	return tokens;
}
