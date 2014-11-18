#ifndef	FINITE_SDL_UTIL_H
#define	FINITE_SDL_UTIL_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_net.h>

#define ALPHA_R		0xff
#define ALPHA_G		0x00
#define ALPHA_B		0xff
#define SCREEN_BPP	32

typedef enum
{
	keyStateUntouched = 0,
	keyStatePressed,
	keyStateHeld,
	keyStateReleased,
	keyStateCount
} KeyStateType;

typedef struct
{
	KeyStateType type;
	float time;
} KeyState;

typedef enum
{
	axisStateInDeadZone = 0,
	axisStateExitDeadZone,
	axisStateOutDeadZone,
	axisStateEnterDeadZone,
	axisStateCount
} AxisStateType;

typedef struct
{
	AxisStateType type;
	float deadZone;
	float value;
	float time;
} AxisState;

void surfaceFree(SDL_Surface *s);
SDL_Surface *surfaceLoad(const char *filename);
SDL_Surface *surfaceAlphaLoad(const char *filename);
void delay(int st, int et, int fps);
void surfaceDraw(SDL_Surface *s0, SDL_Surface *s1, int x, int y);
SDL_Surface *surfaceCpy(SDL_Surface *s);

void axisStateUpdate(AxisState *as);

#endif

