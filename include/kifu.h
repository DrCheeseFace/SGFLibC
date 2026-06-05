#ifndef KIFU_H
#define KIFU_H
#include <sgf.h>

struct SGF_Kifu {
	int c;
};

struct SGF_Kifu *SGF_kifu_create(SGF_Sgf *sgf);

#endif
