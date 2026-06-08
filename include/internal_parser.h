#ifndef INTERNAL_PARSER_H
#define INTERNAL_PARSER_H

#include <internal_tokenizer.h>
#include <mr_utils.h>
#include <sgf.h>

#define SGF_COMMENT_PROPERTY_KEY "C"

SGF_Tokens SGF_internal_get_property_tokens(SGF_Tokens tokens,
					    enum SGF_Property property);

// property expects either SGF_PROPERTIES_AB OR SGF_PROPERTIES_AW
void SGF_internal_init_locations_by_property(MrvVector *tokens,
					     enum SGF_Property property,
					     struct SGF_Move **dest_locations,
					     uint16_t *dest_len);

void SGF_internal_init_RU(SGF_Tokens tokens, enum SGF_Ruleset *dest);
void SGF_internal_init_KM(SGF_Tokens tokens, float *dest);
void SGF_internal_init_single_value_uint16_property(SGF_Tokens tokens,
						    enum SGF_Property property,
						    uint16_t *dest);
void SGF_internal_init_single_value_uint8_property(SGF_Tokens tokens,
						   enum SGF_Property property,
						   uint8_t *dest);
void SGF_internal_init_single_value_str_property(SGF_Tokens tokens,
						 enum SGF_Property property,
						 char **dest);
void SGF_internal_init_RE(SGF_Tokens tokens, SGF_Result *dest);
void SGF_internal_init_variations(SGF_Tokens tokens,
				  struct SGF_MoveNode ***variations,
				  uint16_t *variations_len);
void SGF_internal_free_move_tree(struct SGF_MoveNode *move);

#endif
