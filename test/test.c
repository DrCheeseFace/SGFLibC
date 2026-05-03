#include <stdlib.h>
#include <string.h>

#include <internal_tokenizer.h>
#include <mr_utils.h>
#include <mr_utils/mrt_test.h>
#include <sgf.h>

#define ASSERT_TOKEN(vec, idx, expected_type, expected_text)                   \
	do {                                                                   \
		SGF_Token *_t = (SGF_Token *)mrv_get_idx((vec), (idx));        \
		MRT_ASSERT(_t != NULL, "Check: Token exists at index " #idx);  \
		if (_t != NULL) {                                              \
			MRT_ASSERT(_t->type == (expected_type),                \
				   "Verify: Type is " #expected_type);         \
			MRT_ASSERT(strcmp(_t->text, (expected_text)) == 0,     \
				   "Verify: Text is \"" expected_text "\"");   \
		}                                                              \
	} while (0)

MRT_TEST_GROUP(test_sanity_check)
{
	void *out = SGF_read(NULL);
	MRT_ASSERT(out != NULL, "sgf_read null safety");
	free(out);
}

MRT_TEST_GROUP(test_tokenizer_basic_syntax)
{
	const char *basic_sgf = "(;SZ[19])";
	MrvVector *tokens = SGF_internal_tokeize(basic_sgf);

	MRT_ASSERT(tokens != NULL, "tokenizer allocation");
	MRT_ASSERT(tokens->len == 5, "basic token count");

	ASSERT_TOKEN(tokens, 0, SGF_TOKEN_PAREN_OPEN, "(");
	ASSERT_TOKEN(tokens, 1, SGF_TOKEN_SEMICOLON, ";");
	ASSERT_TOKEN(tokens, 2, SGF_TOKEN_PROPERTY, "SZ");
	ASSERT_TOKEN(tokens, 3, SGF_TOKEN_VALUE, "19");
	ASSERT_TOKEN(tokens, 4, SGF_TOKEN_PAREN_CLOSE, ")");

	SGF_internal_tokens_destroy(tokens);
}

MRT_TEST_GROUP(test_tokenizer_escaped_chars)
{
	const char *escape_sgf = "(;C[Escaped \\] and backslash \\\\ test])";
	MrvVector *tokens = SGF_internal_tokeize(escape_sgf);

	MRT_ASSERT(tokens != NULL, "tokenizer escape support");
	MRT_ASSERT(tokens->len == 5, "escaped token count");
	ASSERT_TOKEN(tokens, 3, SGF_TOKEN_VALUE,
		     "Escaped \\] and backslash \\\\ test");
	SGF_internal_tokens_destroy(tokens);
}

MRT_TEST_GROUP(test_tokenizer_multi_value)
{
	const char *multi_sgf = "(;AB[aa][bb][cc])";
	MrvVector *tokens = SGF_internal_tokeize(multi_sgf);

	MRT_ASSERT(tokens != NULL, "tokenizer multi value support");
	MRT_ASSERT(tokens->len == 7, "multi token count");
	ASSERT_TOKEN(tokens, 2, SGF_TOKEN_PROPERTY, "AB");
	ASSERT_TOKEN(tokens, 3, SGF_TOKEN_VALUE, "aa");
	ASSERT_TOKEN(tokens, 4, SGF_TOKEN_VALUE, "bb");
	ASSERT_TOKEN(tokens, 5, SGF_TOKEN_VALUE, "cc");
	SGF_internal_tokens_destroy(tokens);
}

MRT_TEST_GROUP(test_tokenizer_nested_variations)
{
	const char *nested_sgf = "(;B[pd](;W[dp])(;W[pp]))";
	MrvVector *tokens = SGF_internal_tokeize(nested_sgf);

	MRT_ASSERT(tokens != NULL, "tokenizer nesting support");
	// ( ; B [pd] ( ; W [dp] ) ( ; W [pp] ) )
	// 0 1 2 3    4 5 6 7    8 9 10 11 12 13 14
	MRT_ASSERT(tokens->len == 15, "nested tree total token count");

	ASSERT_TOKEN(tokens, 0, SGF_TOKEN_PAREN_OPEN, "(");
	ASSERT_TOKEN(tokens, 1, SGF_TOKEN_SEMICOLON, ";");
	ASSERT_TOKEN(tokens, 2, SGF_TOKEN_PROPERTY, "B");
	ASSERT_TOKEN(tokens, 3, SGF_TOKEN_VALUE, "pd");

	ASSERT_TOKEN(tokens, 4, SGF_TOKEN_PAREN_OPEN, "(");
	ASSERT_TOKEN(tokens, 5, SGF_TOKEN_SEMICOLON, ";");
	ASSERT_TOKEN(tokens, 6, SGF_TOKEN_PROPERTY, "W");
	ASSERT_TOKEN(tokens, 7, SGF_TOKEN_VALUE, "dp");
	ASSERT_TOKEN(tokens, 8, SGF_TOKEN_PAREN_CLOSE, ")");

	ASSERT_TOKEN(tokens, 9, SGF_TOKEN_PAREN_OPEN, "(");
	ASSERT_TOKEN(tokens, 10, SGF_TOKEN_SEMICOLON, ";");
	ASSERT_TOKEN(tokens, 11, SGF_TOKEN_PROPERTY, "W");
	ASSERT_TOKEN(tokens, 12, SGF_TOKEN_VALUE, "pp");
	ASSERT_TOKEN(tokens, 13, SGF_TOKEN_PAREN_CLOSE, ")");

	ASSERT_TOKEN(tokens, 14, SGF_TOKEN_PAREN_CLOSE, ")");

	SGF_internal_tokens_destroy(tokens);
}

int main(void)
{
	MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
	MrtContext *ctx = mrt_ctx_create(logger);

	MRT_REGISTER_TEST_GROUP(ctx, test_sanity_check);
	MRT_REGISTER_TEST_GROUP(ctx, test_tokenizer_basic_syntax);
	MRT_REGISTER_TEST_GROUP(ctx, test_tokenizer_escaped_chars);
	MRT_REGISTER_TEST_GROUP(ctx, test_tokenizer_multi_value);
	MRT_REGISTER_TEST_GROUP(ctx, test_tokenizer_nested_variations);

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
