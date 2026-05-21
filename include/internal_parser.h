
#ifndef INTERNAL_PARSER_H
#define INTERNAL_PARSER_H

#include <internal_tokenizer.h>
#include <mr_utils.h>
#include <sgf.h>

MrvVector *SGF_internal_get_property_tokens(MrvVector *tokens,
					    enum SGF_Property property);

void SGF_internal_init_location(SGF_Token location_token,
				struct SGF_Location *dest);

// property expects either SGF_PROPERTIES_AB OR SGF_PROPERTIES_AW
void SGF_internal_init_locations_by_property(
	MrvVector *tokens, enum SGF_Property property,
	struct SGF_Location **dest_locations, uint8_t *dest_len);

void SGF_internal_init_RU(MrvVector *tokens, enum SGF_Ruleset *dest);
void SGF_internal_init_single_value_str_property(MrvVector *tokens,
						 enum SGF_Property property,
						 char **dest);

#endif
