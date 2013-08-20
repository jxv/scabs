#ifndef FINITE_GUI_H
#define FINITE_GUI_H

#include <dosk.h>

#include "sdl.h"
#include "common.h"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define FPS 30
#define SPF (1.f/FPS)

#define MAX_GUI_VOLUME	11

typedef enum
{
	audioChanMusic = 0,
	audioChanSfx,
	audioChanCount
} AudioChanType;

typedef enum
{
	yesNoInvalid = -1,
	yes = 0,
	no = 1,
	yesNoCount
} YesNoType;

typedef enum
{
	choiceInvalid = -1,
	choiceRecall = 0,
	choiceMode,
	choicePlay,
	choiceShuffle,
	choiceCount
} ChoiceType;

typedef enum
{
	modeInvalid = -1,
	modePlace = 0,
	modeDiscard,
	modeSkip,
	modeCount
} ModeType;

typedef enum
{
	cmdInvalid = -1,
	cmdFocusTop = 0,
	cmdFocusBottom,
	cmdBoardSelect,
	cmdRackSelect,
	cmdBoard,
	cmdBoardLeft,
	cmdBoardRight,
	cmdBoardUp,
	cmdBoardDown,
	cmdRack,
	cmdRackLeft,
	cmdRackRight,
	cmdChoice,
	cmdChoiceLeft,
	cmdChoiceRight,
	cmdRecall,
	cmdModeUp,
	cmdModeDown,
	cmdModeToggle,
	cmdPlay,
	cmdShuffle,
	cmdBoardCancel,
	cmdRackCancel,
	cmdChoiceCancel,
	cmdTilePrev,
	cmdTileNext,
	cmdQuit,
	cmdCount
} CmdType;

typedef enum
{
	transMoveInvalid = -1,
	transMoveNone = 0,
	transMovePlace,
	transMovePlaceWild,
	transMovePlaceEnd,
	transMovePlacePlay,
	transMoveDiscard,
	transMoveDiscardPlay,
	transMoveSkip,
	transMoveSkipPlay,
	transMoveQuit,
	transMoveCount
} TransMoveType;

typedef enum
{
	tileLookDisable = 0,
	tileLookNormal,
	tileLookHold,
	tileLookGhost,
	tileLookCount
} TileLookType;

typedef enum
{
	menuFocusInvalid = -1,
	menuFocusPlay = 0,
	menuFocusRules,
	menuFocusSettings,
	menuFocusExit,
	menuFocusCount
} MenuFocusType;

typedef enum
{
	gameMenuFocusInvalid = -1,
	gameMenuFocusResume = 0,
	gameMenuFocusSettings,
	gameMenuFocusSkip,
	gameMenuFocusQuit,
	gameMenuFocusCount
} GameMenuFocusType;

typedef enum
{
	settingsFocusInvalid = -1,
	settingsFocusMusic,
	settingsFocusSfx,
	settingsFocusControls,
	settingsFocusCount
} SettingsFocusType;

typedef enum
{
	playMenuFocusInvaid = -1,
	playMenuFocusHumanVsAI = 0,
	playMenuFocusHumanVsHuman,
	playMenuFocusNetplay,
	playMenuFocusCount
} PlayMenuFocusType;


struct MenuWidget;

typedef struct Font
{
	int width;
	int height;
	int spacing;
	SDL_Surface *map;
} Font;

typedef struct HighText
{
	SDL_Surface *normal;
	SDL_Surface *highlight;
	int offset;
} HighText;

typedef struct MenuView
{
	Coor pos;
	Coor spacing;
	HighText *text;
	int len;
	struct MenuWidget *menu;
} MenuView;

typedef struct ScoreCounter
{
	float curTime;
	float endTime;
	int start;
	int cur;
	int end;
	bool stable;
} ScoreCounter;

typedef struct ScoreBoard
{
	int turn;
	int playerNum;
	ScoreCounter ctr[MAX_PLAYER];
	float speed;
	bool stable;
} ScoreBoard;

typedef struct IO
{
	float time;
	SDL_Surface *screen;
	SDL_Surface *back;
	SDL_Surface *fader;
	SDL_Surface *menuBg;
	SDL_Surface *tile[tileCount][letterCount][tileLookCount];
	SDL_Surface *wild[tileLookCount];
	SDL_Surface *wildUp;
	SDL_Surface *wildDown;
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
	SDL_Surface *shuffle;
	SDL_Surface *shuffleDisable;
	SDL_Surface *sq[sqCount];
	SDL_Surface *areYouSureQuit;
	SDL_Surface *titleScreen;
	SDL_Surface *titleHover;
	SDL_Surface *titleBackground;
	SDL_Surface *pressStart;

	SDL_Joystick *joystick;
	
	MenuView menuMV;
	MenuView gameMenuMV;
	MenuView playMenuMV;
	MenuView settingsMV;
	MenuView yesNoMV;
	
	HighText menuFocus[menuFocusCount];
	HighText gameMenuFocus[gameMenuFocusCount];
	HighText playMenuFocus[playMenuFocusCount];
	HighText yesNo[yesNoCount];
	HighText settingsFocus[settingsFocusCount];

	Font whiteFont;
	Font blackFont;
	Font yellowFont;
	Font darkRedFont;

	Font normalFont;
	Font highlightFont;

	Mix_Chunk *incorrectSnd;
	Mix_Chunk *correctSnd;
} IO;

typedef struct KeyState
{
	KeyStateType type;
	float time;
} KeyState;

typedef struct Controls
{
	KeyState start;
	KeyState select;
	KeyState up;
	KeyState down;
	KeyState left;
	KeyState right;
	KeyState a;
	KeyState b;
	KeyState x;
	KeyState y;
	KeyState l;
	KeyState r;
	float axisX;
	float axisY;
} Controls;

typedef struct Cmd
{
	CmdType type;
	union {
	Coor board;
	int rack;
	int choice;
	} data;
} Cmd;

typedef struct GridWidget
{
	Coor index;
	bool **button;
	int width;
	int height;
} GridWidget;

typedef enum
{
	gameGUIFocusInvalid = -1,
	gameGUIFocusBoard = 0,
	gameGUIFocusRack,
	gameGUIFocusChoice,
	gameGUIFocusCount
} GameGUIFocusType;

typedef enum
{
	guiFocusInvalid = -1,
	guiFocusTitle,
	guiFocusMenu,
	guiFocusSettings,
	guiFocusPlayMenu,
	guiFocusGameGUI,
	guiFocusGameMenu,
	guiFocusGameHotseatPause,
	guiFocusGameAIPause,
	guiFocusGameOver,
	guiFocusGameAreYouSureQuit,
	guiFocusCount
} GUIFocusType;

typedef struct MenuWidget
{
	int focus;
	int max;
	int init;
} MenuWidget;

typedef struct GameGUI
{
	GameGUIFocusType focus;
	GameGUIFocusType bottomLast;
	GridWidget rackWidget;
	GridWidget boardWidget;
	GridWidget choiceWidget;
	YesNoType validPlay;
} GameGUI;

typedef struct Settings 
{
	MenuWidget menu;
	int sfxVolume;
	int musVolume;
	GUIFocusType previous;
} Settings;

typedef struct MoveModePlace
{
	int num;
	int idx;
	bool taken[BOARD_Y][BOARD_X];
	int rackIdx[BOARD_Y][BOARD_X];
	Coor boardIdx[RACK_SIZE];
} MoveModePlace;

typedef struct MoveModeDiscard
{
	int num;
	bool rack[RACK_SIZE];
} MoveModeDiscard;

typedef struct TransMove
{
	TransMoveType type;
	int playerIdx;
	Adjust adjust;
	MoveModePlace place;
	MoveModeDiscard discard;
} TransMove;

typedef struct GUI
{
	GUIFocusType focus;
	GUIFocusType next;
	GameGUI gameGui;
	MenuWidget menu;
	MenuWidget gameMenu;
	MenuWidget playMenu;
	Settings settings;
	MenuWidget gameAreYouSureQuit;
	TransMove transMove;
	ScoreBoard scoreBoard;
} GUI;

typedef struct Env
{
	IO io;
	Game game;
	Controls controls;
	GUI gui;
	bool quit;
} Env;

int gui();

void initGame1vs1Human(Game *g);
void initGame1vs1HumanAI(Game *g);
void initScoreBoard(ScoreBoard *sb, Game *g);

void keyStateUpdate(KeyState *ks, bool);

#define TILE_WIDTH 14
#define TILE_HEIGHT 14

#endif

