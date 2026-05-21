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

void SGF_internal_init_locations_by_property(
	MrvVector *tokens, enum SGF_Property property,
	struct SGF_Location **dest_locations, uint8_t *dest_len)
{
	MrvVector *prop_tokens =
		SGF_internal_get_property_tokens(tokens, property);
	if (prop_tokens != NULL) {
		// TODO null check
		*dest_locations =
			calloc(prop_tokens->len, sizeof(**dest_locations));
		*dest_len = prop_tokens->len;

		for (uint i = 0; i < prop_tokens->len; i++) {
			SGF_Token *token = mrv_get_idx(prop_tokens, i);
			SGF_internal_init_location(*token,
						   &(*dest_locations)[i]);
		}

		SGF_internal_tokens_destroy(prop_tokens);
	} else {
		*dest_len = 0;
	}
}

void SGF_internal_init_RU(MrvVector *tokens, enum SGF_Ruleset *dest)
{
	*dest = SGF_RULESET_NONE;

	MrvVector *RU_property_tokens =
		SGF_internal_get_property_tokens(tokens, SGF_PROPERTIES_RU);

	if (RU_property_tokens != NULL) {
		if (RU_property_tokens->len != 1) {
			SGF_internal_tokens_destroy(RU_property_tokens);
			return;
		}

		SGF_Token *ruleset_value_token =
			mrv_get_idx(RU_property_tokens, 0);

		// loop doesnt check SGF_RULESET_NONE
		for (uint i = SGF_RULESET_NONE + 1; i < SGF_RULESET_COUNT;
		     i++) {
			if (strcmp(SGF_ruleset_key[i],
				   ruleset_value_token->text) == 0) {
				*dest = i;
				break;
			}
		}

		SGF_internal_tokens_destroy(RU_property_tokens);
	}
}

// TODO err handling?
void SGF_internal_init_single_value_str_property(MrvVector *tokens,
						 enum SGF_Property property,
						 char **dest)
{
	MrvVector *property_tokens =
		SGF_internal_get_property_tokens(tokens, property);
	if (property_tokens != NULL) {
		if (property_tokens->len != 1) {
			SGF_internal_tokens_destroy(property_tokens);
			return;
		}

		SGF_Token *value_token = mrv_get_idx(property_tokens, 0);

		*dest = malloc(strlen(value_token->text) + 1);
		strcpy(*dest, value_token->text);

		SGF_internal_tokens_destroy(property_tokens);
	}
}
