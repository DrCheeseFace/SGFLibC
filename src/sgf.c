#include <stdlib.h>

#include <internal_parser.h>
#include <internal_tokenizer.h>
#include <mr_utils.h>
#include <sgf.h>
#include <string.h>

const char *SGF_property_key[SGF_PROPERTIES_COUNT] = {
	"W",  "B",  "AB", "AW", "AN", "AP", "BR", "BT", "C",  "CP", "DT",
	"EV", "FF", "GN", "OT", "PB", "PC", "PW", "RO", "SO", "US", "WR",
	"WT", "RE", "KM", "TM", "PL", "RU", "GM", "HA", "ON", "SZ",
};

const char *SGF_ruleset_key[SGF_RULESET_COUNT] = { "None", "Chinese",
						   "Japanese" };

SGF_Sgf *SGF_read(unused FILE *file)
{
	if (!file) {
		return NULL;
	}

	SGF_Sgf *sgf = malloc(sizeof(SGF_Sgf));
	memset(sgf, 0, sizeof(*sgf));

	long length;
	char *buffer = 0;

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = malloc(length);
	if (buffer) {
		fread(buffer, 1, length, file);
	}

	MrvVector *tokens = SGF_internal_tokeize(buffer);
	free(buffer);

	SGF_internal_init_AB_locations(sgf, tokens);
	SGF_internal_init_AW_locations(sgf, tokens);
	SGF_internal_init_RU(sgf, tokens);

	SGF_internal_tokens_destroy(tokens);
	return sgf;
}

void SGF_destroy(SGF_Sgf *sgf)
{
	if (sgf->AB) {
		free(sgf->AB);
	}

	if (sgf->AW) {
		free(sgf->AW);
	}

	free(sgf);
}
