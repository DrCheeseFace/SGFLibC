#ifndef SGF_H
#define SGF_H

#include <stdint.h>
#include <stdio.h>

enum SGF_Player { SGF_PLAYER_BLACK, SGF_PLAYER_WHITE };
enum SGF_Ruleset { SGF_RULESET_CHINESE, SGF_RULESET_JAPANESE };

enum SGF_ResultTag {
	SGF_RESULT_TYPE_RESIGNATION,
	SGF_RESULT_TYPE_SCORE,
};

typedef struct {
	enum SGF_Player player;

	enum SGF_ResultTag tag;
	union {
		uint8_t Score;
		char Resignation;
	} score;
} Result;

struct SGF_Location {
	uint8_t row;
	uint8_t col;
};

struct SGF_Move {
	enum SGF_Player player;
	struct SGF_Location loc;
	struct SGF_Move **variations;
	uint8_t variations_len;
};

enum SGF_Property {
	SGF_PROPERTIES_W, // white move
	SGF_PROPERTIES_B, // black move
	SGF_PROPERTIES_AB, // Add Black: locations of Black stones to be placed on the board prior to the first move
	SGF_PROPERTIES_AW, // Add White: locations of White stones to be placed on the board prior to the first move.
	SGF_PROPERTIES_AN, // Annotations: name of the person commenting the game.
	SGF_PROPERTIES_AP, // Application: application that was used to create the SGF file
	SGF_PROPERTIES_BR, // Black Rank: rank of the Black player.
	SGF_PROPERTIES_BT, // Black Team: name of the Black team.
	SGF_PROPERTIES_C, // Comment: a comment.
	SGF_PROPERTIES_CP, // Copyright: copyright information.
	SGF_PROPERTIES_DT, // Date: date of the game.
	SGF_PROPERTIES_EV, // Event: name of the event
	SGF_PROPERTIES_FF, // File format: version of SGF specification governing this SGF file.
	SGF_PROPERTIES_GN, // Game Name: name of the game record.
	SGF_PROPERTIES_OT, // Overtime: overtime system.
	SGF_PROPERTIES_PB, // Black Name: name of the black player.
	SGF_PROPERTIES_PC, // Place: place where the game was played
	SGF_PROPERTIES_PW, // White Name: name of the white player.
	SGF_PROPERTIES_RO, // Round: round
	SGF_PROPERTIES_SO, // Source: source of the SGF file.
	SGF_PROPERTIES_US, // User: name of the person who created the SGF file.
	SGF_PROPERTIES_WR, // White Rank: rank of the White player.
	SGF_PROPERTIES_WT, // White Team: name of the White team.
	SGF_PROPERTIES_RE, // Result: result, usually in the format "B+R" or "B+3.5".
	SGF_PROPERTIES_KM, // Komi: komi.
	SGF_PROPERTIES_TM, // Time limit: time limit in seconds.
	SGF_PROPERTIES_PL, // Player: color of player to start.
	SGF_PROPERTIES_RU, // Rules: ruleset
	SGF_PROPERTIES_GM, // Game: type of game represented by this SGF file. A property value of 1 refers to Go.
	SGF_PROPERTIES_HA, // Handicap: the number of handicap stones given to Black.

	SGF_PROPERTIES_ON, // Opening: information about the opening; TEMP UNUSED
	SGF_PROPERTIES_SZ, // Size: size of the board, non-square boards are supported. TEMP UNUSED

	SGF_PROPERTIES_COUNT,
};

extern const char *SGF_PropertyKey[SGF_PROPERTIES_COUNT];

typedef struct {
	// Add Black: locations of Black stones to be placed on the board prior to the first move
	struct SGF_Location *AB;
	// Add White: locations of White stones to be placed on the board prior to the first move.
	struct SGF_Location *AW;
	// Annotations: name of the person commenting the game.
	char *AN;
	// Application: application that was used to create the SGF file
	char *AP;

	// variations
	struct SGF_Move **variations;
	uint8_t variations_len;

	// Black Rank: rank of the Black player.
	char *BR;
	// Black Team: name of the Black team.
	char *BT;
	// Comment: a comment.
	char *C;
	// Copyright: copyright information.
	char *CP;
	// Date: date of the game.
	char *DT;
	// Event: name of the event
	char *EV;
	// File format: version of SGF specification governing this SGF file.
	char *FF;
	// Game Name: name of the game record.
	char *GN;
	// Overtime: overtime system.
	char *OT;
	// Black Name: name of the black player.
	char *PB;
	// Place: place where the game was played
	char *PC;
	// White Name: name of the white player.
	char *PW;
	// Round: round
	char *RO;
	// Source: source of the SGF file.
	char *SO;
	// User: name of the person who created the SGF file.
	char *US;
	// White Rank: rank of the White player.
	char *WR;
	// White Team: name of the White team.
	char *WT;

	// Result: result, usually in the format "B+R" or "B+3.5".
	Result RE;
	// Komi: komi.
	float KM;
	// Time limit: time limit in seconds.
	uint32_t TM;
	// Player: color of player to start.
	enum SGF_Player PL;
	// Rules: ruleset
	enum SGF_Ruleset RU;
	// Number of initial Black stones
	uint8_t AB_len;
	// Number of initial White stones
	uint8_t AW_len;
	// Game: type of game represented by this SGF file. A property value of 1 refers to Go.
	uint8_t GM; // need this?
	// Handicap: the number of handicap stones given to Black.
	uint8_t HA;

	// ON 	// Opening: information about the opening
	// SZ 	// Size: size of the board, non-square boards are supported.

} SGF_Sgf; // https://en.wikipedia.org/wiki/Smart_Game_Format

SGF_Sgf *SGF_read(FILE *file);
void SGF_destroy(SGF_Sgf *sgf);

#endif // SGF_H
