#ifndef KIFU_H
#define KIFU_H
#include <sgf.h>
#include <stdint.h>

struct SGF_Kifu {
	struct SGF_Move *moves;
	uint16_t move_count;

	struct SGF_Move *AB;
	struct SGF_Move *AW;
	// Name of annotator
	char *AN;
	// Date
	char *DT;
	// Event
	char *EV;
	// Game Name
	char *GN;
	// Overtime
	char *OT;
	// Black Name
	char *PB;
	// White Name
	char *PW;
	// Round
	char *RO;
	// White Rank
	char *WR;
	// Komi
	float KM;

	// Result
	struct SGF_Result RE;
	// Time limit
	uint16_t TM;
	// Number of initial Black stones
	uint16_t AB_len;
	// Number of initial White stones
	uint16_t AW_len;
	// Handicap
	uint8_t HA;
};

struct SGF_Kifu *SGF_kifu_create(SGF_Sgf *sgf);
void SGF_kifu_destory(struct SGF_Kifu *kifu) __attribute__((nonnull(1)));

#endif
