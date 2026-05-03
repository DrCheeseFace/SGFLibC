#include <stdlib.h>

#include <mr_utils.h>
#include <sgf.h>
#include <string.h>

const char *SGF_PropertyKey[SGF_PROPERTIES_COUNT] = {
	"W",  "B",  "AB", "AW", "AN", "AP", "BR", "BT", "C",  "CP", "DT",
	"EV", "FF", "GN", "OT", "PB", "PC", "PW", "RO", "SO", "US", "WR",
	"WT", "RE", "KM", "TM", "PL", "RU", "GM", "HA", "ON", "SZ",
};

SGF_Sgf *SGF_read(unused FILE *file)
{
	SGF_Sgf *sgf = malloc(sizeof(SGF_Sgf));
	memset(sgf, 0, sizeof(*sgf));

	return sgf;
}

void SGF_destroy(SGF_Sgf *sgf)
{
	free(sgf);
}

// mr_internal char *SGF_get_property(unused FILE *file,
// 				   enum SGF_Property property)
// {
// 	switch (property) {
// 	case SGF_PROPERTIES_W:
// 	case SGF_PROPERTIES_B:
// 	case SGF_PROPERTIES_AB:
// 	case SGF_PROPERTIES_AW:
// 	case SGF_PROPERTIES_C:
// 		return NULL;
// 	default:
// 		break;
// 	}
// 	// AB AW W B C
// 	return NULL;
// }
