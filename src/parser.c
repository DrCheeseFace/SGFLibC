#include <mr_utils.h>

#include <internal_tokenizer.h>
#include <sgf.h>
#include <stdlib.h>
#include <string.h>

MrvVector *SGF_internal_get_property_tokens(MrvVector *tokens,
					    enum SGF_Property property)
{
	int property_token_index = NOT_FOUND;

	const char *property_key = SGF_property_key[property];
	for (uint i = 0; i < tokens->len; i++) {
		SGF_Token *token = mrv_get_idx(tokens, i);

		if ((token->type == SGF_TOKEN_PROPERTY) &&
		    (strcmp(token->text, property_key) == 0)) {
			property_token_index = i;
			break;
		}
	}

	if (property_token_index == NOT_FOUND) {
		return NULL;
	}

	MrvVector *property_tokens = mrv_create(8, sizeof(SGF_Token));
	for (uint i = property_token_index + 1; i < tokens->len; i++) {
		SGF_Token *token = mrv_get_idx(tokens, i);
		if (token->type == SGF_TOKEN_PAREN_OPEN ||
		    token->type == SGF_TOKEN_SEMICOLON ||
		    token->type == SGF_TOKEN_PROPERTY) {
			break;
		}

		SGF_Token t;
		SGF_internal_token_init(&t, token->type, token->text,
					strlen(token->text));
		mrv_append(property_tokens, &t, APPEND_SCALING_INCREMENT);
	}

	return property_tokens;
}

void SGF_internal_init_location(SGF_Token location_token,
				struct SGF_Location *dest)
{
	// char 'a' = 97
	dest->col = location_token.text[0] - 96;
	dest->row = location_token.text[1] - 96;
}

void SGF_internal_init_AB_locations(SGF_Sgf *sgf, MrvVector *tokens)
{
	MrvVector *AB_tokens =
		SGF_internal_get_property_tokens(tokens, SGF_PROPERTIES_AB);
	if (AB_tokens != NULL) {
		// TODO null check
		sgf->AB = calloc(AB_tokens->len, sizeof(*sgf->AB));
		sgf->AB_len = AB_tokens->len;

		for (uint i = 0; i < AB_tokens->len; i++) {
			SGF_Token *token = mrv_get_idx(AB_tokens, i);
			SGF_internal_init_location(*token, &sgf->AB[i]);
		}

		SGF_internal_tokens_destroy(AB_tokens);
	}
}

void SGF_internal_init_AW_locations(SGF_Sgf *sgf, MrvVector *tokens)
{
	MrvVector *AW_tokens =
		SGF_internal_get_property_tokens(tokens, SGF_PROPERTIES_AW);
	if (AW_tokens != NULL) {
		// TODO null check
		sgf->AW = calloc(AW_tokens->len, sizeof(*sgf->AW));
		sgf->AW_len = AW_tokens->len;

		for (uint i = 0; i < AW_tokens->len; i++) {
			SGF_Token *token = mrv_get_idx(AW_tokens, i);
			SGF_internal_init_location(*token, &sgf->AW[i]);
		}

		SGF_internal_tokens_destroy(AW_tokens);
	}
}

void SGF_internal_init_RU(SGF_Sgf *sgf, MrvVector *tokens)
{
	MrvVector *RU_property_tokens =
		SGF_internal_get_property_tokens(tokens, SGF_PROPERTIES_RU);
	if (RU_property_tokens != NULL) {
		SGF_Token *ruleset_value_token =
			mrv_get_idx(RU_property_tokens, 0);

		sgf->RU = SGF_RULESET_NONE;
		// loop doesnt check SGF_RULESET_NONE
		for (uint i = SGF_RULESET_NONE + 1; i < SGF_RULESET_COUNT;
		     i++) {
			if (strcmp(SGF_ruleset_key[i],
				   ruleset_value_token->text) == 0) {
				sgf->RU = i;
				break;
			}
		}
	}

	SGF_internal_tokens_destroy(RU_property_tokens);
}
