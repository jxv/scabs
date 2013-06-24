#ifndef FINITE_GUI_H
#define FINITE_GUI_H

#include "sdl.h"
#include "common.h"


#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240


typedef enum
{
	KEYSTATE_UNTOUCHED = 0,
	KEYSTATE_PRESSED,
	KEYSTATE_HELD,
	KEYSTATE_RELEASED,
	KEYSTATE_COUNT
} KeystateType;


typedef enum
{
	CHOICE_INVALID = -1,
	CHOICE_RECALL = 0,
	CHOICE_MODE,
	CHOICE_PLAY,
	CHOICE_COUNT
} ChoiceType;


typedef enum
{
	MODE_INVALID = -1,
	MODE_PLACE = 0,
	MODE_DISCARD,
	MODE_SKIP,
	MODE_COUNT
} ModeType;


typedef enum
{
	CMD_INVALID = -1,
	CMD_FOCUS_PREV = 0,
	CMD_FOCUS_NEXT,
	CMD_BOARD,
	CMD_RACK,
	CMD_RECALL,
	CMD_MODE_UP,
	CMD_MODE_DOWN,
	CMD_PLAY,
	CMD_QUIT,
	CMD_COUNT
} CmdType;


typedef enum
{
	TRANS_MOVE_INVALID = -1,
	TRANS_MOVE_NONE = 0,
	TRANS_MOVE_PLACE_INIT,
	TRANS_MOVE_PLACE,
	TRANS_MOVE_PLACE_HOLD,
	TRANS_MOVE_DISCARD_INIT,
	TRANS_MOVE_DISCARD,
	TRANS_MOVE_SKIP,
	TRANS_MOVE_QUIT,
	TRANS_MOVE_COUNT
} TransMoveType;


struct font
{
	int 	width;
	int 	height;
	SDL_Surface *map;
};


struct io
{
	SDL_Surface *screen;
	SDL_Surface *back;
	SDL_Surface *tile[TILE_COUNT][LETTER_COUNT];
	SDL_Surface *wild;
	SDL_Surface *lockon;
	SDL_Surface *recall;
	SDL_Surface *recallDisable;
	SDL_Surface *mode;
	SDL_Surface *modeDisable;
	SDL_Surface *place;
	SDL_Surface *placeDisable;
	SDL_Surface *discard;
	SDL_Surface *discardDisable;
	SDL_Surface *skip;
	SDL_Surface *skipDisable;
	SDL_Surface *play;
	SDL_Surface *playDisable;
	struct font white_font;
	struct font black_font;
};


struct keystate
{
	KeystateType type;
	float 	time;
};


struct controls
{
	struct keystate up;
	struct keystate down;
	struct keystate left;
	struct keystate right;
	struct keystate a;
	struct keystate b;
	struct keystate x;
	struct keystate y;
};


struct cmd
{
	CmdType 	type;
	union {
		struct coor board;
		int rack;
	} data;
};


struct gridWidget
{
	struct coor index;
	bool **button;
	int width;
	int height;
};


typedef enum
{
	GUI_FOCUS_INVALID = -1,
	GUI_FOCUS_BOARD = 0,
	GUI_FOCUS_RACK,
	GUI_FOCUS_CHOICE,
	GUI_FOCUS_COUNT
} GuiFocusType;


struct gameGui
{
	GuiFocusType focus;
	struct gridWidget rackWidget;
	struct gridWidget boardWidget;
	struct gridWidget choiceWidget;
};


struct gui
{
	struct gameGui gameGui;
};


struct moveModePlace
{
	int num;
	int idx;
};


struct moveModeDiscard
{
	bool rack[RACK_SIZE];
};


struct transMove
{
	TransMoveType type;
	int playerIdx;
	struct adjust adjust;
	union {
		struct moveModePlace place;
		struct moveModeDiscard discard;
	} data;
};


struct env
{
	struct io io;
	struct game game;
	struct controls controls;
	struct gui gui;
	struct transMove transMove;
};


int gui();


#endif


