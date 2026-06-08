#include <stdlib.h>
#include <string.h>

#include <kifu.h>
#include <mr_utils.h>

struct SGF_Kifu *
SGF_kifu_create(SGF_Sgf *sgf)
{
	struct SGF_Kifu *kifu = malloc(sizeof(*kifu));
	if (sgf->AN != NULL) {
		kifu->AN = malloc(strlen(sgf->AN) + 1);
		strcpy(kifu->AN, sgf->AN);
	}

	if (sgf->DT != NULL) {
		kifu->DT = malloc(strlen(sgf->DT) + 1);
		strcpy(kifu->DT, sgf->DT);
	}

	if (sgf->EV != NULL) {
		kifu->EV = malloc(strlen(sgf->EV) + 1);
		strcpy(kifu->EV, sgf->EV);
	}

	if (sgf->GN != NULL) {
		kifu->GN = malloc(strlen(sgf->GN) + 1);
		strcpy(kifu->GN, sgf->GN);
	}

	if (sgf->OT != NULL) {
		kifu->OT = malloc(strlen(sgf->OT) + 1);
		strcpy(kifu->OT, sgf->OT);
	}

	if (sgf->PB != NULL) {
		kifu->PB = malloc(strlen(sgf->PB) + 1);
		strcpy(kifu->PB, sgf->PB);
	}

	if (sgf->PW != NULL) {
		kifu->PW = malloc(strlen(sgf->PW) + 1);
		strcpy(kifu->PW, sgf->PW);
	}

	if (sgf->RO != NULL) {
		kifu->RO = malloc(strlen(sgf->RO) + 1);
		strcpy(kifu->RO, sgf->RO);
	}

	if (sgf->WR != NULL) {
		kifu->WR = malloc(strlen(sgf->WR) + 1);
		strcpy(kifu->WR, sgf->WR);
	}

	kifu->KM = sgf->KM;
	kifu->RE = sgf->RE;
	kifu->TM = sgf->TM;
	kifu->AB_len = sgf->AB_len;
	kifu->AW_len = sgf->AW_len;
	kifu->HA = sgf->HA;

	// TODO
	// kifu->move_count

	return kifu;
}

void
SGF_kifu_destory(struct SGF_Kifu *kifu)
{
	//TODO
	free(kifu);
}
