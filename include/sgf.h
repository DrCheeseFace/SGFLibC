#ifndef SGF_H
#define SGF_H

#include <stdio.h>

typedef struct {
        int x;
}Sgf;

Sgf* sgf_read(FILE * file);

#endif // SGF_H
