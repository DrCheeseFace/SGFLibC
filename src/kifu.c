#include <stdlib.h>

#include <kifu.h>
#include <mr_utils.h>

struct SGF_Kifu *
SGF_kifu_create(SGF_Sgf *sgf)
{
	ignore sgf;
	return NULL;
}

void
SGF_kifu_destory(struct SGF_Kifu *kifu)
{
	//TODO
	free(kifu);
}
