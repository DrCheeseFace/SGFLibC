
#ifndef INTERNAL_PARSER_H
#define INTERNAL_PARSER_H

#include <internal_tokenizer.h>
#include <mr_utils.h>
#include <sgf.h>

MrvVector *SGF_internal_get_property_tokens(MrvVector *tokens,
					    enum SGF_Property property);

void SGF_internal_init_location(SGF_Token location_token,
			       struct SGF_Location *dest);

void SGF_internal_init_AB_locations(SGF_Sgf *sgf, MrvVector *tokens);
void SGF_internal_init_AW_locations(SGF_Sgf *sgf, MrvVector *tokens);
void SGF_internal_init_RU(SGF_Sgf *sgf, MrvVector *tokens);

#endif
