#ifndef KIFU_H
#define KIFU_H
#include <sgf.h>
#include <stdio.h>
struct SGF_Kifu {
	int c;

};

void SGF_draw_kifu(SGF_Sgf *sgf, FILE *out);

#endif
