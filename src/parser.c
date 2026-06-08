#include <mr_utils.h>

#include <internal_parser.h>
#include <internal_tokenizer.h>
#include <sgf.h>
#include <stdlib.h>
#include <string.h>

internal struct SGF_MoveNode *SGF_internal_parse_node_stream(SGF_Tokens tokens,
							     size_t *index);
void SGF_internal_init_prev_link(struct SGF_MoveNode *move);

SGF_Tokens
SGF_internal_get_property_tokens(SGF_Tokens tokens, enum SGF_Property property)
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

	SGF_Tokens property_tokens = mrv_create(8, sizeof(SGF_Token));
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

internal void
SGF_internal_init_location(SGF_Token location_token, struct SGF_Move *dest)
{
	// TODO handle numbers here
	// char 'a' = 97
	dest->col = location_token.text[0] - 96;
	dest->row = location_token.text[1] - 96;
}

void
SGF_internal_init_locations_by_property(SGF_Tokens tokens,
					enum SGF_Property property,
					struct SGF_Move **dest_locations,
					uint16_t *dest_len)
{
	SGF_Tokens prop_tokens =
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

void
SGF_internal_init_KM(SGF_Tokens tokens, float *dest)
{
	SGF_Tokens KM_property_tokens =
		SGF_internal_get_property_tokens(tokens, SGF_PROPERTIES_KM);

	if (KM_property_tokens->len != 1) {
		SGF_internal_tokens_destroy(KM_property_tokens);
		return;
	}

	SGF_Token *komi_property_token = mrv_get_idx(KM_property_tokens, 0);
	*dest = strtof(komi_property_token->text, NULL);

	SGF_internal_tokens_destroy(KM_property_tokens);
}

void
SGF_internal_init_single_value_uint16_property(SGF_Tokens tokens,
					       enum SGF_Property property,
					       uint16_t *dest)
{
	SGF_Tokens property_tokens =
		SGF_internal_get_property_tokens(tokens, property);

	if (property_tokens->len != 1) {
		SGF_internal_tokens_destroy(property_tokens);
		return;
	}

	SGF_Token *property_token = mrv_get_idx(property_tokens, 0);

	unsigned long raw_value = strtoul(property_token->text, NULL, 10);
	if (raw_value > UINT16_MAX) {
		return;
	}

	*dest = (uint16_t)raw_value;

	SGF_internal_tokens_destroy(property_tokens);
}

void
SGF_internal_init_single_value_uint8_property(SGF_Tokens tokens,
					      enum SGF_Property property,
					      uint8_t *dest)
{
	SGF_Tokens property_tokens =
		SGF_internal_get_property_tokens(tokens, property);

	if (property_tokens->len != 1) {
		SGF_internal_tokens_destroy(property_tokens);
		return;
	}

	SGF_Token *property_token = mrv_get_idx(property_tokens, 0);

	unsigned long raw_value = strtoul(property_token->text, NULL, 10);
	if (raw_value > UINT8_MAX) {
		return;
	}

	*dest = (uint8_t)raw_value;

	SGF_internal_tokens_destroy(property_tokens);
}

void
SGF_internal_init_RU(SGF_Tokens tokens, enum SGF_Ruleset *dest)
{
	*dest = SGF_RULESET_NONE;

	SGF_Tokens RU_property_tokens =
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

void
SGF_internal_init_RE(SGF_Tokens tokens, SGF_Result *dest)
{
	dest->player = SGF_PLAYER_NONE;
	dest->type = SGF_RESULT_TYPE_UNKNOWN;

	SGF_Tokens RE_property_tokens =
		SGF_internal_get_property_tokens(tokens, SGF_PROPERTIES_RE);

	if (RE_property_tokens == NULL) {
		return;
	}

	if (RE_property_tokens->len != 1) {
		SGF_internal_tokens_destroy(RE_property_tokens);
		return;
	}

	SGF_Token *result_token = mrv_get_idx(RE_property_tokens, 0);
	const char *text = result_token->text;

	// FF[4] only
	switch (text[0]) {
	case '0':
	case 'D':
		if (strcmp(text, "0") == 0 || strcmp(text, "Draw") == 0) {
			dest->type = SGF_RESULT_TYPE_DRAW;
		}
		break;

	case 'V':
		if (strcmp(text, "Void") == 0) {
			dest->type = SGF_RESULT_TYPE_NO_RESULT;
		}
		break;

	case '?':
		dest->type = SGF_RESULT_TYPE_UNKNOWN;
		break;

	case 'B':
	case 'W':
		dest->player =
			(text[0] == 'B') ? SGF_PLAYER_BLACK : SGF_PLAYER_WHITE;

		if (text[1] == '+') {
			const char *reason = text + 2;

			switch (reason[0]) {
			case 'R':
				dest->type = SGF_RESULT_TYPE_RESIGNATION;
				break;
			case 'T':
				dest->type = SGF_RESULT_TYPE_TIME;
				break;
			case 'F':
				dest->type = SGF_RESULT_TYPE_FORFEIT;
				break;
			case '\0':
				dest->type = SGF_RESULT_TYPE_UNKNOWN;
				break;
			default:
				dest->type = SGF_RESULT_TYPE_SCORE;
				dest->score = strtof(reason, NULL);
				break;
			}
		}
		break;

	default:
		dest->type = SGF_RESULT_TYPE_UNKNOWN;
		break;
	}

	SGF_internal_tokens_destroy(RE_property_tokens);
}

// TODO err handling?
void
SGF_internal_init_single_value_str_property(SGF_Tokens tokens,
					    enum SGF_Property property,
					    char **dest)
{
	SGF_Tokens property_tokens =
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

internal void
SGF_internal_tokens_trim_setup_node(SGF_Tokens tokens)
{
	SGF_internal_token_free((void *)tokens->arr);
	mrv_pop_front(tokens); // removed '(' token

	SGF_internal_token_free((void *)tokens->arr);
	mrv_pop_front(tokens); // removed ';' token

	SGF_Token *token = mrv_get_last(tokens);
	SGF_internal_token_free(token);
	mrv_pop(tokens); // remove ending ')'

	for (;;) {
		token = mrv_get_idx(tokens, 0);
		if (token->type == SGF_TOKEN_SEMICOLON) {
			break;
		}

		SGF_internal_token_free(token);
		mrv_pop_front(tokens);
		if (tokens->len == 0) {
			break;
		}
	}

	return;
}

// figure that one out sports fans
// TODO max depth for variations?
void
SGF_internal_init_variations(SGF_Tokens tokens,
			     struct SGF_MoveNode ***variations,
			     uint16_t *variations_len)
{
	SGF_Tokens trimed_tokens = SGF_internal_tokens_dupe(tokens);
	SGF_internal_tokens_trim_setup_node(trimed_tokens);

	if (trimed_tokens->len == 0) {
		*variations = NULL;
		*variations_len = 0;
		SGF_internal_tokens_destroy(trimed_tokens);
		return;
	}

	size_t index = 0;
	SGF_Token *next = mrv_get_idx(trimed_tokens, index);

	if (next->type == SGF_TOKEN_SEMICOLON) {
		struct SGF_MoveNode *next_node =
			SGF_internal_parse_node_stream(trimed_tokens, &index);
		if (next_node) {
			*variations = malloc(sizeof(struct SGF_MoveNode *));
			(*variations)[0] = next_node;
			*variations_len = 1;
		}
	} else if (next->type == SGF_TOKEN_PAREN_OPEN) {
		size_t cap = 4;
		*variations = malloc(cap * sizeof(struct SGF_MoveNode *));
		*variations_len = 0;

		while (index < trimed_tokens->len) {
			SGF_Token *chk = mrv_get_idx(trimed_tokens, index);
			if (chk->type != SGF_TOKEN_PAREN_OPEN)
				break;

			index++;
			struct SGF_MoveNode *sub_node =
				SGF_internal_parse_node_stream(trimed_tokens,
							       &index);
			if (sub_node) {
				if (*variations_len >= cap) {
					cap *= 2;
					*variations = realloc(
						*variations,
						cap * sizeof(struct SGF_MoveNode
								     *));
				}
				(*variations)[(*variations_len)++] = sub_node;
			}

			if (index < trimed_tokens->len) {
				SGF_Token *close =
					mrv_get_idx(trimed_tokens, index);
				if (close->type == SGF_TOKEN_PAREN_CLOSE) {
					index++;
				}
			}
		}
	}

	for (uint16_t i = 0; i < *variations_len; i++) {
		(*variations)[i]->prev = NULL;
		SGF_internal_init_prev_link((*variations)[i]);
	}

	SGF_internal_tokens_destroy(trimed_tokens);
}

void
SGF_internal_init_prev_link(struct SGF_MoveNode *move)
{
	if (move->variations == NULL) {
		return;
	}

	for (uint16_t i = 0; i < move->variations_len; i++) {
		move->variations[i]->prev = move;
		SGF_internal_init_prev_link(move->variations[i]);
	}
}

void
SGF_internal_free_move_tree(struct SGF_MoveNode *move)
{
	if (!move)
		return;
	if (move->comment) {
		free(move->comment);
	}
	if (move->variations) {
		for (uint16_t i = 0; i < move->variations_len; i++) {
			SGF_internal_free_move_tree(move->variations[i]);
		}
		free(move->variations);
	}
	free(move);
}

internal struct SGF_MoveNode *
SGF_internal_parse_node_stream(SGF_Tokens tokens, size_t *index)
{
	if (*index >= tokens->len)
		return NULL;

	SGF_Token *tok = mrv_get_idx(tokens, *index);
	if (tok->type != SGF_TOKEN_SEMICOLON)
		return NULL;

	(*index)++; // consume ';'

	struct SGF_MoveNode *node = calloc(1, sizeof(struct SGF_MoveNode));
	node->move.player = SGF_PLAYER_NONE;
	node->comment = NULL;

	while (*index < tokens->len) {
		SGF_Token *prop = mrv_get_idx(tokens, *index);
		if (prop->type != SGF_TOKEN_PROPERTY)
			break;

		const char *key = prop->text;
		(*index)++;

		while (*index < tokens->len) {
			SGF_Token *val = mrv_get_idx(tokens, *index);
			if (val->type != SGF_TOKEN_VALUE)
				break;

			if (strcmp(key, SGF_player_key[SGF_PLAYER_BLACK]) ==
			    0) {
				node->move.player = SGF_PLAYER_BLACK;
				SGF_internal_init_location(*val, &node->move);
			} else if (strcmp(key,
					  SGF_player_key[SGF_PLAYER_WHITE]) ==
				   0) {
				node->move.player = SGF_PLAYER_WHITE;
				SGF_internal_init_location(*val, &node->move);
			} else if (strcmp(key, SGF_COMMENT_PROPERTY_KEY) == 0) {
				node->comment = malloc(strlen(val->text) + 1);
				if (node->comment)
					strcpy(node->comment, val->text);
			}

			(*index)++;
		}
	}

	if (*index < tokens->len) {
		SGF_Token *next = mrv_get_idx(tokens, *index);

		if (next->type == SGF_TOKEN_SEMICOLON) {
			struct SGF_MoveNode *next_node =
				SGF_internal_parse_node_stream(tokens, index);
			if (next_node) {
				node->variations =
					malloc(sizeof(struct SGF_MoveNode *));
				node->variations[0] = next_node;
				node->variations_len = 1;
			}
		} else if (next->type == SGF_TOKEN_PAREN_OPEN) {
			size_t cap = 4;
			node->variations =
				malloc(cap * sizeof(struct SGF_MoveNode *));

			while (*index < tokens->len) {
				SGF_Token *chk = mrv_get_idx(tokens, *index);
				if (chk->type != SGF_TOKEN_PAREN_OPEN)
					break;

				(*index)++; // skip '('
				struct SGF_MoveNode *sub_node =
					SGF_internal_parse_node_stream(tokens,
								       index);
				if (sub_node) {
					if (node->variations_len >= cap) {
						cap *= 2;
						node->variations = realloc(
							node->variations,
							cap * sizeof(struct SGF_MoveNode
									     *));
					}
					node->variations[node->variations_len++] =
						sub_node;
				}

				if (*index < tokens->len) {
					SGF_Token *close =
						mrv_get_idx(tokens, *index);
					if (close->type ==
					    SGF_TOKEN_PAREN_CLOSE) {
						(*index)++;
					}
				}
			}
		}
	}

	return node;
}
