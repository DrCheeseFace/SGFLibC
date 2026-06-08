#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <internal_parser.h>
#include <internal_tokenizer.h>
#include <mr_utils.h>
#include <mr_utils/mrt_test.h>
#include <sgf.h>

global_variable const char *EXAMPLE_SGF =
	"(;AP[HexGui:0.10.GIT]FF[4]GM[11]SZ[7]C[Example game]"
	" ;B[aa]C[This opening is too strong. White will definitely swap it.]"
	" ;W[bb];B[cc];W[dd];B[ee];W[ff]C[Good.]"
	" ;B[gg];W[hh];B[ii];W[jj]"
	" (;B[ab]C[See the next variation for what happens if Black plays b4.]"
	"  ;W[ac];B[ad];W[ae];B[resign];)"
	" (;B[af];W[ag]"
	"  ;AB[ba][be][bf][bg][bh][fh][jk][lk][da][ce][fe]"
	"  C[Note that White is connected by templates, requiring only the area shown.])"
	")";

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
	MRT_ASSERT(out == NULL, "sgf_read null safety");
}

MRT_TEST_GROUP(test_tokenizer_basic_syntax)
{
	const char *basic_sgf = "(;SZ[19])";
	SGF_Tokens tokens = SGF_internal_tokeize(basic_sgf);

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
	SGF_Tokens tokens = SGF_internal_tokeize(escape_sgf);

	MRT_ASSERT(tokens != NULL, "tokenizer escape support");
	MRT_ASSERT(tokens->len == 5, "escaped token count");
	ASSERT_TOKEN(tokens, 3, SGF_TOKEN_VALUE,
		     "Escaped \\] and backslash \\\\ test");
	SGF_internal_tokens_destroy(tokens);
}

MRT_TEST_GROUP(test_tokenizer_multi_value)
{
	const char *multi_sgf = "(;AB[aa][bb][cc])";
	SGF_Tokens tokens = SGF_internal_tokeize(multi_sgf);

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
	SGF_Tokens tokens = SGF_internal_tokeize(nested_sgf);

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

MRT_TEST_GROUP(test_get_property_tokens)
{
	const char *sgf =
		"PW[whiteplayername]PB[blackplayername]AB[pd][dp][pp](;W[dd]";
	SGF_Tokens tokens = SGF_internal_tokeize(sgf);

	SGF_Tokens PW_tokens =
		SGF_internal_get_property_tokens(tokens, SGF_PROPERTIES_PW);
	ASSERT_TOKEN(PW_tokens, 0, SGF_TOKEN_VALUE, "whiteplayername");
	MRT_ASSERT(PW_tokens->len == 1, "PW property tokens length");
	SGF_internal_tokens_destroy(PW_tokens);

	SGF_Tokens AB_tokens =
		SGF_internal_get_property_tokens(tokens, SGF_PROPERTIES_AB);
	ASSERT_TOKEN(AB_tokens, 0, SGF_TOKEN_VALUE, "pd");
	ASSERT_TOKEN(AB_tokens, 1, SGF_TOKEN_VALUE, "dp");
	ASSERT_TOKEN(AB_tokens, 2, SGF_TOKEN_VALUE, "pp");
	MRT_ASSERT(AB_tokens->len == 3, "AB property tokens length");
	SGF_internal_tokens_destroy(AB_tokens);

	SGF_internal_tokens_destroy(tokens);
}

MRT_TEST_GROUP(test_init_AW_AB_locations)
{
	struct SGF_Move *locations = NULL;
	uint16_t len = 0;
	const char *sgf_str =
		"PW[whiteplayername]PB[blackplayername]AB[aa][ab][cd](;W[dd]";
	SGF_Tokens tokens = SGF_internal_tokeize(sgf_str);

	SGF_internal_init_locations_by_property(tokens, SGF_PROPERTIES_AW,
						&locations, &len);
	MRT_ASSERT(len == 0, "AW location length");
	MRT_ASSERT(locations == NULL, "AW locations null");

	SGF_internal_init_locations_by_property(tokens, SGF_PROPERTIES_AB,
						&locations, &len);
	MRT_ASSERT(len, "AB locations length");
	MRT_ASSERT(locations[0].col == 1, "AB location 1 col ");
	MRT_ASSERT(locations[0].row == 1, "AB location 1 row");
	MRT_ASSERT(locations[1].col == 1, "AB location 2 col ");
	MRT_ASSERT(locations[1].row == 2, "AB location 2 row");
	MRT_ASSERT(locations[2].col == 3, "AB location 3 col ");
	MRT_ASSERT(locations[2].row == 4, "AB location 3 row");

	SGF_internal_tokens_destroy(tokens);
	free(locations);

	locations = NULL;
	len = 0;
	sgf_str = "PW[whiteplayername]PB[blackplayername]AW[aa][ab][cd](;W[dd]";
	tokens = SGF_internal_tokeize(sgf_str);

	SGF_internal_init_locations_by_property(tokens, SGF_PROPERTIES_AB,
						&locations, &len);
	MRT_ASSERT(len == 0, "AB location length");
	MRT_ASSERT(locations == NULL, "AB locations null");

	SGF_internal_init_locations_by_property(tokens, SGF_PROPERTIES_AW,
						&locations, &len);
	MRT_ASSERT(len == 3, "AW locations length");
	MRT_ASSERT(locations[0].col == 1, "AW location 1 col ");
	MRT_ASSERT(locations[0].row == 1, "AW location 1 row");
	MRT_ASSERT(locations[1].col == 1, "AW location 2 col ");
	MRT_ASSERT(locations[1].row == 2, "AW location 2 row");
	MRT_ASSERT(locations[2].col == 3, "AW location 3 col ");
	MRT_ASSERT(locations[2].row == 4, "AW location 3 row");

	SGF_internal_tokens_destroy(tokens);
	free(locations);
}

MRT_TEST_GROUP(test_init_RU)
{
	enum SGF_Ruleset out_ruleset;
	const char *sgf_str = "SZ[19]HA[3]RU[Japanese]KM[6.50]";
	SGF_Tokens tokens = SGF_internal_tokeize(sgf_str);

	SGF_internal_init_RU(tokens, &out_ruleset);
	MRT_ASSERT(out_ruleset == SGF_RULESET_JAPANESE,
		   "init ruleset japanese");

	SGF_internal_tokens_destroy(tokens);

	sgf_str = "SZ[19]HA[3]RU[Chinese]KM[6.50]";
	tokens = SGF_internal_tokeize(sgf_str);

	SGF_internal_init_RU(tokens, &out_ruleset);
	MRT_ASSERT(out_ruleset == SGF_RULESET_CHINESE, "init ruleset chinese");

	SGF_internal_tokens_destroy(tokens);

	sgf_str = "SZ[19]HA[3]KM[6.50]";
	tokens = SGF_internal_tokeize(sgf_str);

	SGF_internal_init_RU(tokens, &out_ruleset);
	MRT_ASSERT(out_ruleset == SGF_RULESET_NONE, "init ruleset not set");

	SGF_internal_tokens_destroy(tokens);

	sgf_str = "SZ[19]RU[meatballs]HA[3]KM[6.50]";
	tokens = SGF_internal_tokeize(sgf_str);

	SGF_internal_init_RU(tokens, &out_ruleset);
	MRT_ASSERT(out_ruleset == SGF_RULESET_NONE, "init ruleset invalid");

	SGF_internal_tokens_destroy(tokens);
}

MRT_TEST_GROUP(test_init_KM)
{
	float out_komi;
	const char *sgf_str = "SZ[19]HA[3]RU[Japanese]KM[6.50]";
	SGF_Tokens tokens = SGF_internal_tokeize(sgf_str);

	SGF_internal_init_KM(tokens, &out_komi);
	MRT_ASSERT(fabsf(out_komi - 6.5f) < FLT_EPSILON, "positive komi");

	SGF_internal_tokens_destroy(tokens);

	sgf_str = "SZ[19]HA[3]RU[Chinese]KM[-6.50]";
	tokens = SGF_internal_tokeize(sgf_str);

	SGF_internal_init_KM(tokens, &out_komi);
	MRT_ASSERT(fabsf(out_komi + 6.5f) < FLT_EPSILON, "negative komi");

	SGF_internal_tokens_destroy(tokens);
}

MRT_TEST_GROUP(test_init_single_value_str_property)
{
	char *out_val = NULL;
	const char *sgf_str = "(;GM[1]FF[4]CA[UTF-8]AP[CGoban:3]ST[2]";
	SGF_Tokens tokens = SGF_internal_tokeize(sgf_str);

	SGF_internal_init_single_value_str_property(tokens, SGF_PROPERTIES_AP,
						    &out_val);

	MRT_ASSERT(out_val != NULL, "init property not NULL");
	MRT_ASSERT(strcmp(out_val, "CGoban:3") == 0, "init property value");

	SGF_internal_tokens_destroy(tokens);
	free(out_val);

	out_val = NULL;
	sgf_str = "(;GM[1]FF[4]CA[UTF-8]ST[2]";
	tokens = SGF_internal_tokeize(sgf_str);

	SGF_internal_init_single_value_str_property(tokens, SGF_PROPERTIES_AP,
						    &out_val);
	MRT_ASSERT(out_val == NULL, "init property undefined");

	SGF_internal_tokens_destroy(tokens);
}

MRT_TEST_GROUP(test_move_tree_parsing)
{
	SGF_Tokens tokens = SGF_internal_tokeize(EXAMPLE_SGF);
	struct SGF_MoveNode **variations = NULL;
	uint16_t variations_len = 0;

	SGF_internal_init_variations(tokens, &variations, &variations_len);
	MRT_ASSERT(variations != NULL, "variations array allocated");
	MRT_ASSERT(
		variations_len == 1,
		"root setup node transitions to exactly one game track path");

	struct SGF_MoveNode *move_1 = variations[0];
	MRT_ASSERT(move_1 != NULL, "first move exists");

	MRT_ASSERT(move_1->prev == NULL, "root move prev pointer is null");

	MRT_ASSERT(move_1->move.player == SGF_PLAYER_BLACK,
		   "move 1 player is black");
	MRT_ASSERT(move_1->move.col == 1 && move_1->move.row == 1,
		   "move 1 location maps to [aa] -> (1,1)");
	MRT_ASSERT(move_1->comment != NULL, "move 1 comment string not null");
	MRT_ASSERT(
		strcmp(move_1->comment,
		       "This opening is too strong. White will definitely swap it.") ==
			0,
		"move 1 comment string check");
	MRT_ASSERT(move_1->variations_len == 1, "move 1 continues linearly");

	// skip down across the continuous, linear main branch segment down to move 10 (;W[jj])
	struct SGF_MoveNode *curr = move_1;
	for (int i = 0; i < 9; i++) {
		MRT_ASSERT(curr->variations_len == 1,
			   "sequence path remains linear");

		struct SGF_MoveNode *next_move = curr->variations[0];
		MRT_ASSERT(next_move->prev == curr, "check prev node");

		curr = next_move;
	}

	// verify move 10 (;W[jj]) before branching
	MRT_ASSERT(curr->move.player == SGF_PLAYER_WHITE,
		   "move 10 player is white");
	MRT_ASSERT(curr->move.col == 10 && curr->move.row == 10,
		   "move 10 location maps to [jj] -> (10,10)");
	MRT_ASSERT(curr->variations_len == 2,
		   "move 10 correctly forks into two distinct variations");
	MRT_ASSERT(curr->comment == NULL, "move 10 no comment");

	// branch A check
	struct SGF_MoveNode *branch_a = curr->variations[0];
	MRT_ASSERT(branch_a != NULL, "branch A tracking initialized");
	MRT_ASSERT(branch_a->prev == curr,
		   "branch A prev pointer points back to move 10");
	MRT_ASSERT(branch_a->move.player == SGF_PLAYER_BLACK,
		   "branch A move 1 player is black");
	MRT_ASSERT(branch_a->move.col == 1 && branch_a->move.row == 2,
		   "branch A move 1 maps to [ab] -> (1,2)");

	// branch B check
	struct SGF_MoveNode *branch_b = curr->variations[1];
	MRT_ASSERT(branch_b != NULL, "branch B tracking initialized");
	MRT_ASSERT(branch_b->prev == curr,
		   "branch B prev pointer points back to Move 10");
	MRT_ASSERT(branch_b->move.player == SGF_PLAYER_BLACK,
		   "branch B move 1 player is black");
	MRT_ASSERT(branch_b->move.col == 1 && branch_b->move.row == 6,
		   "branch B move 1 maps to [af] -> (1,6)");

	for (uint16_t i = 0; i < variations_len; i++) {
		SGF_internal_free_move_tree(variations[i]);
	}
	free(variations);
	SGF_internal_tokens_destroy(tokens);
}

MRT_TEST_GROUP(test_init_RE)
{
	struct SGF_Result out_result;
	const char *sgf_str;
	SGF_Tokens tokens;

	// 0
	sgf_str = "RE[0]";
	tokens = SGF_internal_tokeize(sgf_str);
	SGF_internal_init_RE(tokens, &out_result);
	MRT_ASSERT(out_result.type == SGF_RESULT_TYPE_DRAW, "RE[0] is Draw");
	SGF_internal_tokens_destroy(tokens);

	// draw
	sgf_str = "RE[Draw]";
	tokens = SGF_internal_tokeize(sgf_str);
	SGF_internal_init_RE(tokens, &out_result);
	MRT_ASSERT(out_result.type == SGF_RESULT_TYPE_DRAW, "RE[Draw] is Draw");
	SGF_internal_tokens_destroy(tokens);

	// void
	sgf_str = "RE[Void]";
	tokens = SGF_internal_tokeize(sgf_str);
	SGF_internal_init_RE(tokens, &out_result);
	MRT_ASSERT(out_result.type == SGF_RESULT_TYPE_NO_RESULT,
		   "RE[Void] is No Result");
	SGF_internal_tokens_destroy(tokens);

	// unknown
	sgf_str = "RE[?]";
	tokens = SGF_internal_tokeize(sgf_str);
	SGF_internal_init_RE(tokens, &out_result);
	MRT_ASSERT(out_result.type == SGF_RESULT_TYPE_UNKNOWN,
		   "RE[?] is Unknown");
	SGF_internal_tokens_destroy(tokens);

	// black resignation
	sgf_str = "RE[B+R]";
	tokens = SGF_internal_tokeize(sgf_str);
	SGF_internal_init_RE(tokens, &out_result);
	MRT_ASSERT(out_result.player == SGF_PLAYER_BLACK,
		   "RE[B+R] player is Black");
	MRT_ASSERT(out_result.type == SGF_RESULT_TYPE_RESIGNATION,
		   "RE[B+R] is Resignation");
	SGF_internal_tokens_destroy(tokens);

	// white Time
	sgf_str = "RE[W+T]";
	tokens = SGF_internal_tokeize(sgf_str);
	SGF_internal_init_RE(tokens, &out_result);
	MRT_ASSERT(out_result.player == SGF_PLAYER_WHITE,
		   "RE[W+T] player is White");
	MRT_ASSERT(out_result.type == SGF_RESULT_TYPE_TIME, "RE[W+T] is Time");
	SGF_internal_tokens_destroy(tokens);

	// Black Forfeit
	sgf_str = "RE[B+F]";
	tokens = SGF_internal_tokeize(sgf_str);
	SGF_internal_init_RE(tokens, &out_result);
	MRT_ASSERT(out_result.player == SGF_PLAYER_BLACK,
		   "RE[B+F] player is Black");
	MRT_ASSERT(out_result.type == SGF_RESULT_TYPE_FORFEIT,
		   "RE[B+F] is Forfeit");
	SGF_internal_tokens_destroy(tokens);

	// white Score
	sgf_str = "RE[W+3.5]";
	tokens = SGF_internal_tokeize(sgf_str);
	SGF_internal_init_RE(tokens, &out_result);
	MRT_ASSERT(out_result.player == SGF_PLAYER_WHITE,
		   "RE[W+3.5] player is White");
	MRT_ASSERT(out_result.type == SGF_RESULT_TYPE_SCORE,
		   "RE[W+3.5] is Score");
	MRT_ASSERT(fabsf(out_result.score - 3.5f) < FLT_EPSILON,
		   "RE[W+3.5] score parsed as float");
	SGF_internal_tokens_destroy(tokens);

	// fallback
	sgf_str = "RE[B+]";
	tokens = SGF_internal_tokeize(sgf_str);
	SGF_internal_init_RE(tokens, &out_result);
	MRT_ASSERT(out_result.player == SGF_PLAYER_BLACK,
		   "RE[B+] player is Black");
	MRT_ASSERT(out_result.type == SGF_RESULT_TYPE_UNKNOWN,
		   "RE[B+] missing reason fall back to unknown tag");
	SGF_internal_tokens_destroy(tokens);
}

MRT_TEST_GROUP(debug_test)
{
	SGF_Tokens tokens = SGF_internal_tokeize(EXAMPLE_SGF);
	struct SGF_MoveNode **variations = NULL;
	uint16_t variations_len = 0;

	SGF_internal_init_variations(tokens, &variations, &variations_len);

	if (variations) {
		for (uint16_t i = 0; i < variations_len; i++) {
			SGF_internal_free_move_tree(variations[i]);
		}
		free(variations);
	}

	SGF_internal_tokens_destroy(tokens);
}

int
main(void)
{
	MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
	MrtContext *ctx = mrt_ctx_create(logger);

	MRT_REGISTER_TEST_GROUP(ctx, test_sanity_check);
	MRT_REGISTER_TEST_GROUP(ctx, test_tokenizer_basic_syntax);
	MRT_REGISTER_TEST_GROUP(ctx, test_tokenizer_escaped_chars);
	MRT_REGISTER_TEST_GROUP(ctx, test_tokenizer_multi_value);
	MRT_REGISTER_TEST_GROUP(ctx, test_tokenizer_nested_variations);
	MRT_REGISTER_TEST_GROUP(ctx, test_get_property_tokens);
	MRT_REGISTER_TEST_GROUP(ctx, test_init_AW_AB_locations);
	MRT_REGISTER_TEST_GROUP(ctx, test_init_RU);
	MRT_REGISTER_TEST_GROUP(ctx, test_init_KM);
	MRT_REGISTER_TEST_GROUP(ctx, test_init_single_value_str_property);
	MRT_REGISTER_TEST_GROUP(ctx, test_move_tree_parsing);
	MRT_REGISTER_TEST_GROUP(ctx, test_init_RE);

	MRT_REGISTER_TEST_GROUP(ctx, debug_test);

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
