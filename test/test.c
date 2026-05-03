#include <stdlib.h>
#include <string.h>

#include <internal_tokenizer.h>
#include <mr_utils.h>
#include <mr_utils/mrt_test.h>
#include <sgf.h>

#define ASSERT_TOKEN(vec, idx, expected_type, expected_text)                   \
	do {                                                                   \
		Token *_t = (Token *)mrv_get_idx((vec), (idx));                \
		MRT_ASSERT(_t != NULL, "Check: Token exists at index " #idx);  \
		if (_t != NULL) {                                              \
			MRT_ASSERT(_t->type == (expected_type),                \
				   "Verify: Type is " #expected_type);         \
			MRT_ASSERT(strcmp(_t->text, (expected_text)) == 0,     \
				   "Verify: Text is \"" expected_text "\"");   \
		}                                                              \
	} while (0)

MRT_TEST_GROUP(sanity_check)
{
	void *out = SGF_read(NULL);
	MRT_ASSERT(out != NULL, "sgf_read null safety");
	free(out);
}

MRT_TEST_GROUP(tokenizer_basic_syntax)
{
	const char *basic_sgf = "(;SZ[19])";
	MrvVector *tokens = tokenize(basic_sgf);

	MRT_ASSERT(tokens != NULL, "tokenizer allocation");
	if (tokens) {
		MRT_ASSERT(tokens->len == 5, "basic token count");

		ASSERT_TOKEN(tokens, 0, TOKEN_PAREN_OPEN, "(");
		ASSERT_TOKEN(tokens, 1, TOKEN_SEMICOLON, ";");
		ASSERT_TOKEN(tokens, 2, TOKEN_PROPERTY, "SZ");
		ASSERT_TOKEN(tokens, 3, TOKEN_VALUE, "19");
		ASSERT_TOKEN(tokens, 4, TOKEN_PAREN_CLOSE, ")");

		tokens_destroy(tokens);
	}
}

MRT_TEST_GROUP(tokenizer_escaped_chars)
{
	const char *escape_sgf = "(;C[Escaped \\] and backslash \\\\ test])";
	MrvVector *tokens = tokenize(escape_sgf);

	MRT_ASSERT(tokens != NULL, "tokenizer escape support");
	if (tokens) {
		MRT_ASSERT(tokens->len == 5, "escaped token count");
		ASSERT_TOKEN(tokens, 3, TOKEN_VALUE,
			     "Escaped \\] and backslash \\\\ test");
		tokens_destroy(tokens);
	}
}

MRT_TEST_GROUP(tokenizer_multi_value)
{
	const char *multi_sgf = "(;AB[aa][bb][cc])";
	MrvVector *tokens = tokenize(multi_sgf);

	MRT_ASSERT(tokens != NULL, "tokenizer multi value support");
	if (tokens) {
		MRT_ASSERT(tokens->len == 7, "multi token count");
		ASSERT_TOKEN(tokens, 2, TOKEN_PROPERTY, "AB");
		ASSERT_TOKEN(tokens, 3, TOKEN_VALUE, "aa");
		ASSERT_TOKEN(tokens, 4, TOKEN_VALUE, "bb");
		ASSERT_TOKEN(tokens, 5, TOKEN_VALUE, "cc");
		tokens_destroy(tokens);
	}
}

MRT_TEST_GROUP(tokenizer_nested_variations)
{
	const char *nested_sgf = "(;B[pd](;W[dp])(;W[pp]))";
	MrvVector *tokens = tokenize(nested_sgf);

	MRT_ASSERT(tokens != NULL, "tokenizer nesting support");
	if (tokens) {
		// ( ; B [pd] ( ; W [dp] ) ( ; W [pp] ) )
		// 0 1 2 3    4 5 6 7    8 9 10 11 12 13 14
		MRT_ASSERT(tokens->len == 15, "nested tree total token count");

		ASSERT_TOKEN(tokens, 0, TOKEN_PAREN_OPEN, "(");
		ASSERT_TOKEN(tokens, 1, TOKEN_SEMICOLON, ";");
		ASSERT_TOKEN(tokens, 2, TOKEN_PROPERTY, "B");
		ASSERT_TOKEN(tokens, 3, TOKEN_VALUE, "pd");

		ASSERT_TOKEN(tokens, 4, TOKEN_PAREN_OPEN, "(");
		ASSERT_TOKEN(tokens, 5, TOKEN_SEMICOLON, ";");
		ASSERT_TOKEN(tokens, 6, TOKEN_PROPERTY, "W");
		ASSERT_TOKEN(tokens, 7, TOKEN_VALUE, "dp");
		ASSERT_TOKEN(tokens, 8, TOKEN_PAREN_CLOSE, ")");

		ASSERT_TOKEN(tokens, 9, TOKEN_PAREN_OPEN, "(");
		ASSERT_TOKEN(tokens, 10, TOKEN_SEMICOLON, ";");
		ASSERT_TOKEN(tokens, 11, TOKEN_PROPERTY, "W");
		ASSERT_TOKEN(tokens, 12, TOKEN_VALUE, "pp");
		ASSERT_TOKEN(tokens, 13, TOKEN_PAREN_CLOSE, ")");

		ASSERT_TOKEN(tokens, 14, TOKEN_PAREN_CLOSE, ")");

		tokens_destroy(tokens);
	}
}

int main(void)
{
	MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
	MrtContext *ctx = mrt_ctx_create(logger);

	MRT_REGISTER_TEST_GROUP(ctx, sanity_check);
	MRT_REGISTER_TEST_GROUP(ctx, tokenizer_basic_syntax);
	MRT_REGISTER_TEST_GROUP(ctx, tokenizer_escaped_chars);
	MRT_REGISTER_TEST_GROUP(ctx, tokenizer_multi_value);
	MRT_REGISTER_TEST_GROUP(ctx, tokenizer_nested_variations);

#ifdef DEBUG
	Err err = mrt_ctx_run(ctx, FALSE);
#else
	Err err = mrt_ctx_run(ctx, TRUE);
#endif

	mrt_ctx_destroy(ctx);
	mrl_destroy(logger);

#ifdef DEBUG
	ASSERT(mrd_log_dump_active_allocations() == 0);
#endif

	return err;
}
