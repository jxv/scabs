#include <string.h>
#include <math.h>

#include "gui.h"
#include "draw.h"
#include "widget.h"

void drawScoreBoard(ScoreBoard *sb, IO *io);

void strDraw(SDL_Surface *s, Font *f, const char *str, int x, int y)
{
	int i;
	char c;
	SDL_Rect offset, clip, prev;

	NOT(s);
	NOT(f);
	NOT(f->map);
	NOT(str);

	offset.x = x;
	offset.y = y;
	clip.y = 0;
	clip.h = f->height;
	clip.w = f->width;
	for (i = 0; str[i] != '\0'; i++) {
		c = str[i];
		if (c == '\n') {
			offset.x = x;
			offset.y += f->height;
			continue;
		}
		/* [32..126] are drawable ASCII chars */
		if (c >= 32 && c <= 126) {
			clip.x = f->width * (c - 32);
			prev = offset;
			SDL_BlitSurface(f->map, &clip, s, &offset);
			offset = prev;
		}
		offset.x += f->width + f->spacing;
	}
}

void drawProgressBar(SDL_Surface *s, float progress, int x, int y, int w, int h, int sp)
{
	SDL_Rect rect;

	NOT(s);

	rect.x = x - sp;
	rect.y = y - sp;
	rect.w = w + sp * 2;
	rect.h = h + sp * 2;
	SDL_FillRect(s, &rect, SDL_MapRGB(s->format, 0x00, 0x00, 0x00));

	rect.x = x;
	rect.y = y;
	rect.w = w * progress;
	rect.h = h;
	SDL_FillRect(s, &rect, SDL_MapRGB(s->format, 0xff, 0xff, 0xff));
}

void printCmd(Cmd *c)
{
	NOT(c);

	switch (c->type) {
	case cmdFocusTop: puts("[cmd:focus-top]"); break;
	case cmdFocusBottom: puts("[cmd:focus-bottom]"); break;
	case cmdBoardSelect: printf("[cmd:board-select (%d,%d)]\n", c->data.board.x, c->data.board.y); break;
	case cmdRackSelect: printf("[cmd:rack-select %d]\n", c->data.rack); break;
	case cmdBoard: printf("[cmd:board (%d, %d)]\n", c->data.board.x, c->data.board.y); break;
	case cmdBoardLeft: puts("[cmd:board-left]"); break;
	case cmdBoardRight: puts("[cmd:board-right]"); break;
	case cmdBoardUp: puts("[cmd:board-up]"); break;
	case cmdBoardDown: puts("[cmd:board-down]"); break;
	case cmdRack: printf("[cmd:rack %d]\n", c->data.rack); break;
	case cmdRackLeft: puts("[cmd:rack-left]"); break;
	case cmdRackRight: puts("[cmd:rack-right]"); break;
	case cmdRecall: puts("[cmd:recall]"); break;
	case cmdMode: puts("[cmd:mode]"); break;
	case cmdPlay: puts("[cmd:play]"); break;
	case cmdShuffle: puts("[cmd:shuffle]"); break;
	case cmdBoardCancel: printf("[cmd:board-cancel (%d,%d)\n]", c->data.board.x, c->data.board.y); break;
	case cmdRackCancel: printf("[cmd:rack-cancel %d]", c->data.rack); break;
	case cmdTilePrev: puts("[cmd:tile-prev]"); break;
	case cmdTileNext: puts("[cmd:tile-next]"); break;
	case cmdQuit: puts("[cmd:quit]"); break;
	/*case cmdInvalid: puts("[cmd:invalid]"); break; // very noisy */
	default: break;
	}
}

void printTransMove(TransMove *tm)
{
	NOT(tm);

	switch (tm->type) {
	case transMoveNone: puts("[trans-move:none]"); break;
	case transMovePlace: puts("[trans-move:place]"); break;
	case transMovePlaceWild: puts("[trans-move:place-wild]"); break;
	case transMovePlaceEnd: puts("[trans-move:place-end]"); break;
	case transMovePlacePlay: puts("[trans-move:place-play]"); break;
	case transMoveDiscard: puts("[trans-move:discard]"); break;
	case transMoveDiscardPlay: puts("[trans-move:discard-play]"); break;
	case transMoveSkip: puts("[trans-move:skip]"); break;
	case transMoveSkipPlay: puts("[trans-move:skip-play]"); break;
	case transMoveQuit: puts("[trans-move:quit]"); break;
	case transMoveInvalid: puts("[trans-move:invalid]"); break;
	default: break;
	}
}

void guiDrawLockon(IO *io, GameGUI *gg)
{
	const int w = TILE_WIDTH;
	const int h = TILE_HEIGHT;
	SDL_Surface *s;

	coor_t idx;

	NOT(io);
	NOT(gg);
	s = interval(io->time, 0.2f) ? io->lockon : io->lockon0;
	switch (gg->focus) {
	case gameGUIFocusBoard: {
		idx = gg->boardWidget.index;
		surface_draw(io->screen, s,
			    gg->boardWidget.pos.x - 2 + idx.x * w,
			    gg->boardWidget.pos.y - 2 + idx.y * h);
		break;
	}
	case gameGUIFocusRack: {
		idx = gg->rackWidget.index;
		surface_draw(io->screen, s,
			    gg->rackWidget.pos.x - 2 + idx.x * w,
			    gg->rackWidget.pos.y - 2);
		break;
	}
	default: break;
	}
}

void guiDrawGhostTile(IO *io, GameGUIFocusType gf, TransMove *tm, player_t *p,
		      GridWidget *bw)
{
	int i;
	coor_t idx;
	SDL_Surface *s;
	tile_t *t;

	NOT(io);
	NOT(tm);
	NOT(p);
	NOT(bw);
	if (gf != gameGUIFocusBoard) {
		return;
	}

	switch (tm->type) {
	case transMovePlace: {
		i = tm->adjust.data.tile[tm->place.idx].idx;
		idx = bw->index;
		t = &p->tile[i];
		if (t->type == TILE_NONE) {
			break;
		}
		s = t->type == TILE_WILD ? io->wild[tileLookGhost] : io->tile[t->type][t->letter][tileLookGhost];
		surface_draw(io->screen, s, idx.x * TILE_WIDTH + bw->pos.x, idx.y * TILE_HEIGHT + bw->pos.y);
		break;
	}
	case transMovePlaceWild: {
		i = tm->adjust.data.tile[tm->place.idx].idx;
		idx = bw->index;
		t = &p->tile[i];
		surface_draw(io->screen, io->wildUp, idx.x * TILE_WIDTH + bw->pos.x,
			(idx.y-1) * TILE_HEIGHT + bw->pos.y + (TILE_HEIGHT/2));
		surface_draw(io->screen, io->wildDown, idx.x * TILE_WIDTH + bw->pos.x,
			(idx.y+1) * TILE_HEIGHT + bw->pos.y);
		break;
	}
	default: break;
	}
}

void guiDrawBoard(IO *io, GridWidget *bw, game_t *g, TransMove *tm, LastMove *lm)
{
	coor_t dim;
	
	NOT(io);
	NOT(bw);
	NOT(g);
	NOT(tm);

	dim.x = TILE_WIDTH;
	dim.y = TILE_HEIGHT;

	boardWidgetDraw(io, bw, &g->player[tm->playerIdx], &g->board, tm, lm, dim);
}

void guiDrawBoardWithoutTransMove(IO *io, GridWidget *bw, game_t *g, LastMove *lm)
{
	coor_t dim;
	
	NOT(io);
	NOT(bw);
	NOT(g);

	dim.x = TILE_WIDTH;
	dim.y = TILE_HEIGHT;

	boardWidgetDrawWithoutTransMove(io, bw, &g->board, lm, dim);
}
void guiDrawRack(IO *io, GridWidget *rw, game_t *g, TransMove *tm)
{
	coor_t dim;
	
	NOT(io);
	NOT(rw);
	NOT(g);
	NOT(tm);

	dim.x = TILE_WIDTH;
	dim.y = TILE_HEIGHT;

	rackWidgetDraw(io, tm, rw, dim, &g->player[tm->playerIdx]);
}

bool interval(float lapsed, float interval)
{
	return interval == 0 ? 0 : ((int)floorf(lapsed / interval)) % 2 == 0;
}

void guiDraw(IO *io, GUI *g, game_t *gm, TransMove *tm, GameControls *gc)
{
	int i, j;

	NOT(io);
	NOT(g);
	NOT(tm);
	NOT(gc);
	
	surface_draw(io->screen, io->gmBack, 0, 0);
	guiDrawBoard(io, &g->gameGui.boardWidget, gm, tm, &g->gameGui.lastMove);
	guiDrawRack(io, &g->gameGui.rackWidget, gm, tm);
	drawScoreBoard(&g->scoreBoard, io);

	
	if (gm->turn == tm->playerIdx && interval(io->time, 0.2f)) {
		guiDrawGhostTile(io, g->gameGui.focus, tm, &gm->player[tm->playerIdx], &g->gameGui.boardWidget);
	}
	guiDrawLockon(io, &g->gameGui);
	if (tm->type == transMoveDiscard || tm->type == transMoveDiscardPlay) {
		tile_t *t;

		strDraw(io->screen, &io->normalFont, "DISCARD", 126 + 12, 8);
		surface_draw(io->screen, io->boardCover, g->gameGui.boardWidget.pos.x, g->gameGui.boardWidget.pos.y);
		
		i = g->gameGui.rackWidget.index.x;
		t = &gm->player[tm->playerIdx].tile[i];
		if (t->type != TILE_NONE) {
			char str0[32];
			SDL_Surface *s;
			s = t->type == TILE_WILD ? io->wild[tileLookNormal] : io->tile[t->type][t->letter][tileLookNormal];
			surface_draw(io->screen, s, 269, 9);
			sprintf(str0,": %d", tile_score(t));
			strDraw(io->screen, &io->normalFont, str0, 281, 8);
		}
	} else {
		coor_t *idx;
		char *str = "\0";
		tile_t *t;

		idx = &g->gameGui.boardWidget.index;

		strDraw(io->screen, &io->normalFont, "PLACE", 132 + 12,  8);
		if (g->gameGui.focus == gameGUIFocusBoard) {
			sq_t sq;
			sq = gm->board.sq[idx->y][idx->x];

			surface_draw(io->screen, io->sq[sq], 190, 9);

			str = "\0";
			switch (sq) {
			case SQ_NORMAL: str = "NO BONUS"; break;
			case SQ_DBL_LET: str = "x2 LETTER"; break;
			case SQ_TRP_LET: str = "x3 LETTER"; break;
			case SQ_DBL_WRD: str = "x2 WORD"; break;
			case SQ_TRP_WRD: str = "x3 WORD"; break;
			case SQ_FREE: str = "FREE SQ."; break;
			case SQ_MYSTERY: str = "MYSTERY"; break;
			case SQ_NO_VOWEL: str = "VOWEL ONLY"; break;
			case SQ_BLOCK: str = "BLOCK"; break;
			default: break;
			}
			strDraw(io->screen, &io->normalFont, str, 203, 8);

		}

		i = tm->adjust.data.tile[tm->place.idx].idx;
		t = &gm->player[tm->playerIdx].tile[i];
		if (t->type != TILE_NONE) {
			char str0[32];
			SDL_Surface *s;
			s = t->type == TILE_WILD ? io->wild[tileLookNormal] : io->tile[t->type][t->letter][tileLookNormal];
			surface_draw(io->screen, s, 269, 9);
			sprintf(str0,": %d", tile_score(t));
			strDraw(io->screen, &io->normalFont, str0, 281, 8);
		}
	}

/* hack for printing bag count */
	{
		int len;
		char tilesLeft[32] = "\0";
		if (!bag_empty(&gm->bag)) {
			sprintf(tilesLeft, "BAG: %d", bag_count(&gm->bag));
		} else {
			int idx;
			assert(gm->turn == 0 || gm->turn == 1);
			idx = gm->turn == 0 ? 1 : 0;
			sprintf(tilesLeft, "PLAYER %d: %d", idx + 1,
                                rack_count(&gm->player[idx]));
		}
		strDraw(io->screen, &io->normalFont, tilesLeft, 13, 82);
		len = strlen(tilesLeft) * (io->normalFont.width + io->normalFont.spacing) + 13;
		len += 6;
		surface_draw(io->screen, io->wild[tileLookNormal], len, 83);
		len += 12;
		strDraw(io->screen, &io->normalFont, "'s", len, 82);
	}
/* hack above */

	for (i = 0, j = g->gameGui.textLog.head; i < g->gameGui.textLog.size; i++, j++, j %= g->gameGui.textLog.size) {
		strDraw(io->screen, &io->blackFont, g->gameGui.textLog.line[j], 16, 84 + 24 + 12 * i);
	}

	if (tm->type == transMovePlace || tm->type == transMovePlacePlay || tm->type == transMovePlaceEnd) {
		surface_draw(io->screen, io->btn[gc->key[gameKeyPrevTile]], 157, 218);
		surface_draw(io->screen, io->btn[gc->key[gameKeyNextTile]], 274, 218);
	}


}

int scrollOffset(int dis, int pps, float time)
{
	int secsToLoop;
	secsToLoop = dis / pps;
	return (int)(dis * time / secsToLoop) % dis;
}

void drawScrollingBackground(IO *io)
{
	int off0, off1;
	
	NOT(io);

	off0 = scrollOffset(io->titleBackground->h, 20, io->time);
	off1 = scrollOffset(io->titleHover->h, -30, io->time);

	surface_draw(io->screen, io->titleBackground, 0, off0);
	surface_draw(io->screen, io->titleBackground, 0, off0 - io->titleBackground->h);
	
	surface_draw(io->screen, io->titleHover, 0, off1);
	surface_draw(io->screen, io->titleHover, 0, off1 + io->titleHover->h);
}

void drawMenuBackground(IO *io)
{
	int off0;
	
	NOT(io);

	off0 = scrollOffset(io->titleBackground->h, 20, io->time);

	surface_draw(io->screen, io->menuBackground, 0, off0);
	surface_draw(io->screen, io->menuBackground, 0, off0 - io->menuBackground->h);
}

void drawNum(SDL_Surface *s, int x, int y, int num, Font *f)
{
	SDL_Rect clip, offset;
	int c, n;

	NOT(s);
	NOT(f);
	NOT(f->map);

	n = num > -num ? num : -num;
	offset.x = x;
	offset.y = y;
	clip.y = 0;
	clip.h = f->height;
	clip.w = f->width;

	if (num == 0) {
		c = '0' - 32;
		clip.x = c * f->width;
		SDL_BlitSurface(f->map, &clip, s, &offset);
		return;
	}

	do {	
		c = '0' + (n % 10) - 32;
		clip.x = c * f->width;
		SDL_BlitSurface(f->map, &clip, s, &offset);
		offset.x -= f->width;
		n /= 10;
	} while (n != 0);

	if (num < 0) {
		c = '-' - 32;
		clip.x = c * f->width;
		SDL_BlitSurface(f->map, &clip, s, &offset);
	} 
}

void drawFader(IO *io, int val)
{
	NOT(io);
	assert(val >= 0);
	assert(val < 256);

	SDL_FillRect(io->fader, 0, SDL_MapRGBA(io->fader->format, 0, 0, 0, val));
	SDL_SetAlpha(io->fader, SDL_SRCALPHA, val);
	surface_draw(io->screen, io->fader, 0, 0);
}

void drawScoreBoard(ScoreBoard *sb, IO *io)
{
	char text[32];
	int i;
	Font *f;

	NOT(sb);
	NOT(io);
	
	surface_draw(io->screen, io->scoreBoard, 8, 6);
	surface_draw(io->screen, io->textLog, 8, 104);
	for (i = 0; i < sb->playerNum; i++) {
		f = i == sb->turn || sb->ctr[i].cur < sb->ctr[i].end 
				? &io->highlightFont 
				: &io->normalFont;
		drawNum(io->screen, 100, (f->height + 1) * i + 11, sb->ctr[i].cur, f);
		sprintf(text, "PLAYER %d", i+1);
		strDraw(io->screen, f, text, 15, (f->height + 1) * i + 11);
	}
}

void drawMenuView(SDL_Surface *s, MenuView *mv)
{
	SDL_Surface *t;
	int i, x, y;

	NOT(s);
	NOT(mv);

	for (i = 0; i < mv->menu->max; i++) {
		t = mv->menu->focus == i ? mv->text[i].highlight : mv->text[i].normal;
		NOT(t);
		x = mv->pos.x + mv->text[i].offset;
		y = mv->pos.y + mv->spacing.y * i;
		surface_draw(s, t, x, y);
	}
}

void drawMenuViewRight(SDL_Surface *s, MenuView *mv)
{
	SDL_Surface *t;
	int i, x, y;

	NOT(s);
	NOT(mv);

	for (i = 0; i < mv->menu->max; i++) {
		t = mv->menu->focus == i ? mv->text[i].highlight : mv->text[i].normal;
		NOT(t);
		x = mv->pos.x + mv->text[i].offset * 2;
		y = mv->pos.y + mv->spacing.y * i;
		surface_draw(s, t, x, y);
	}
}

void drawMenuViewLeft(SDL_Surface *s, MenuView *mv)
{
	SDL_Surface *t;
	int i, x, y;

	NOT(s);
	NOT(mv);

	for (i = 0; i < mv->menu->max; i++) {
		t = mv->menu->focus == i ? mv->text[i].highlight : mv->text[i].normal;
		NOT(t);
		x = mv->pos.x;
		y = mv->pos.y + mv->spacing.y * i;
		surface_draw(s, t, x, y);
	}
}

void draw_guiFocusTitle(Env *e)
{
	NOT(e);

	drawScrollingBackground(&e->io);
	surface_draw(e->io.screen, e->io.titleScreen, 0, 0);
	if ((e->io.time - floorf(e->io.time)) > 0.5) {
		surface_draw(e->io.screen, e->io.pressStart, 128, 200);
	}
	/*drawProgressBar(e->io.screen, 0.5f, 60, 180, 200, 10, 1);*/
}

void draw_guiFocusMenu(Env *e)
{
	NOT(e);

	drawMenuBackground(&e->io);
	surface_draw(e->io.screen, e->io.finiteTitle, 0, 0);
	drawMenuView(e->io.screen, &e->io.menuMV);
}

void draw_guiFocusRules(Env *e)
{
	const int rule_lines = 37;
	const char *rules_text[] = {
		"Objective: Score the most points",
		"by spelling interconnected words.",
		"",
		"Setup:",
		"1 Each player is given a random",
		"  set of 7 letter tiles. Neither",
		"  player knows what tiles the",
		"  other players have.",
		"2 The free square in the center",
		"  must be the location of the",	/* 10 */
		"  initial move.",
		"",
		"Scoring:",
		"3 Spelling a legal word ends",
		"  your turn. Each word is spell-",
		"  checked.",
		"4 Each tile and square have",
		"  defined values which are", 
		"  displayed on the top right.",
		"  The value of a placed word is",	/* 20 */
		"  the [sum of [tiles times the",
		"  letter square bonuses]] which",
		"  is all times the word square",
		"  bonuses.",
		"5 The blank tile has no value,",
		"  but it will attribute to word",
		"  bonuses.",
		"",
		"Discarding:",
		"6 You may discard tiles from the",	/* 30 */
		"  game as a turn. Your discarded",
		"  tiles will be replaced by ",
		"  remaining tiles in the bag.",
		"",
		"End Game:",
		"7 The game ends when there are",
		"  no words to spell.",			/* 37 */
	};
	SDL_Rect rect;
	float scroll;
	int i;

	NOT(e);

	drawMenuBackground(&e->io);

	scroll = -e->gui.rules * 320;

	rect.x = 49;
	rect.y = 0;
	rect.w = SCREEN_WIDTH - rect.x * 2;
	rect.h = SCREEN_HEIGHT;
	SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x50, 0x50, 0xa0, 0xff));

	rect.x = 50;
	rect.y = 0;
	rect.w = SCREEN_WIDTH - rect.x * 2;
	rect.h = SCREEN_HEIGHT;
	SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x20, 0x20, 0x70, 0xff));
	

	rect.x += rect.w - 6;
	rect.w = 3 + 2;
	rect.h = 45 ;
	rect.y = e->gui.rules * (SCREEN_HEIGHT - rect.h - 4) + 2 - 1;
	rect.h = 47;
	SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x00, 0x00, 0x00, 0xff));
	
	rect.x += 1;
	rect.w = 3;
	rect.h = 45;
	rect.y = e->gui.rules * (SCREEN_HEIGHT - rect.h - 4) + 2;
	SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0xe0, 0xa0, 0x00, 0xff));
	rect.w = 1;
	SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0xf0, 0xf0, 0xf0, 0xff));
	rect.x += 2;
	SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0xc0, 0x60, 0x00, 0xff));
	
	surface_draw(e->io.screen, e->io.rulesTitle, 0 , scroll);


	for (i = 0; i < rule_lines; i++) {
		strDraw(e->io.screen, &e->io.normalFont, rules_text[i], 60, scroll + 60 + i * 12);
	}
}

void draw_guiFocusGameGUI(Env *e);

int placeValues(int x)
{
	int i = 0;
	int j = 10;

	for (i = 0; x >= j; i ++) {
		j *= 10;
	}
	
	return i;
}

void draw_guiFocusSettings(Env *e)
{
	Font *f;
	int i;

	NOT(e);
	
	if (e->gui.settings.previous == guiFocusMenu) {
		drawMenuBackground(&e->io);
	} else {
		surface_draw(e->io.screen, e->io.menuBg, 0, 0);
	}
	surface_draw(e->io.screen, e->io.settingsTitle, 0, 0);

	drawMenuView(e->io.screen, &e->io.settingsMV);
	for (i = 0; i < settingsFocusCount; i++) {
		f = i == e->gui.settings.menu.focus ? &e->io.highlightFont : &e->io.normalFont;
		if (i == settingsFocusSfx) {
			int v;
			int ii = interval(e->io.time, 0.2f);
			v = e->gui.settings.vol[i - settingsFocusSfx];
			drawNum(e->io.screen, 183, i * e->io.normalFont.height * 2 + 92, v, f);
			
			if (i == e->gui.settings.menu.focus) {
				if (v > 0) {
					strDraw(e->io.screen, f, "<", 174 - ii - 6 * placeValues(v), i * e->io.normalFont.height * 2 + 92);
				}
				if (v < MAX_GUI_VOLUME) {
					strDraw(e->io.screen, f, ">", 190 + ii, i * e->io.normalFont.height * 2 + 94);
				}
			}
		}
	}
}

void draw_guiFocusControls(Env *e)
{
	static char *hardwareKeyText[hardwareKeyCount] =
	{
		"Start",
		"Select",
		"Up",
		"Down",
		"Left",
		"Right",
		"A",
		"B",
		"X",
		"Y",
		"L",
		"R"
	};

	static char *uknown = "?";
	static char *shuffle = "Shake";
	static char *noShuffle = "Shake N/A";
	
	int i;
	Font *f;

	NOT(e);
	
	if (e->gui.settings.previous == guiFocusMenu) {
		drawMenuBackground(&e->io);
	} else {
		surface_draw(e->io.screen, e->io.menuBg, 0, 0);
	}

	surface_draw(e->io.screen, e->io.controlsTitle, 0, 0);

	drawMenuViewRight(e->io.screen, &e->io.controlsMV);
	
	for (i = 0; i < gameKeyCount; i++) {
		HardwareKeyType hkt;
		char *text = uknown;

		hkt =  e->io.controls.game.key[i];

		if (hkt >= 0 && hkt < hardwareKeyCount) {
			text = hardwareKeyText[hkt];
		}

		if (i == gameKeyShuffle) {
			text = e->io.accelExists ? shuffle : noShuffle;
		}

		f = &e->io.normalFont;
		
		if (e->gui.controlsMenu.menu.focus == i) {
			f = &e->io.highlightFont;
		}


		if (!changableGameKey(e->gui.controlsMenu.menu.focus)) {
			f = &e->io.normalFont;

		} else {
			if (i == e->gui.controlsMenu.menu.focus) {
				int ii = interval(e->io.time, 0.2f);
				strDraw(e->io.screen, f, "<", -ii + 157, e->io.controlsMV.pos.y + i * e->io.controlsMV.spacing.y);
				strDraw(e->io.screen, f, ">", ii + 168 + strlen(text) * (f->width + f->spacing), e->io.controlsMV.pos.y + i * e->io.controlsMV.spacing.y);
			}
		}
		
		if (e->gui.controlsMenu.dupKey[i]) {
			f = &e->io.highlightFont;
		}
		
		strDraw(e->io.screen, f, text, 166, e->io.controlsMV.pos.y + i * e->io.controlsMV.spacing.y);
	}
	
}

void draw_guiFocusGameGUI(Env *e)
{
	NOT(e);

	guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove, &e->io.controls.game); 

	
	if (e->gui.gameGui.lastInput > 7.0f) {
		if (e->gui.transMove.type == transMoveDiscard) {
			int y;
			SDL_Rect rect;
			coor_t *pos;
			
			pos = &e->gui.gameGui.boardWidget.pos;

			rect.w = (e->io.normalFont.width + e->io.normalFont.spacing) * 16 + 6;
			rect.h = e->io.normalFont.height * 5 + 6;

			rect.x = pos->x;
			rect.x += (BOARD_X * TILE_WIDTH - rect.w) / 2;

			rect.y = pos->y;
			rect.y += BOARD_Y * TILE_HEIGHT;
			rect.y -= rect.h;

			rect.x -= 2;
			rect.y -= 2;
			rect.w += 4;
			rect.h += 4;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x20, 0x20, 0x70, 255));

			rect.x += 1;
			rect.y += 1;
			rect.w -= 2;
			rect.h -= 2;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x50, 0x50, 0xa0, 255));

			rect.x += 1;
			rect.y += 1;
			rect.w -= 2;
			rect.h -= 2;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x00, 0x00, 0x00, 255));
			

			y = e->io.normalFont.height + 1;
			rect.x += 5;

			rect.y += y;
			surface_draw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeySelect]], rect.x, rect.y);
			rect.y += y;
			surface_draw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyPlay]], rect.x, rect.y);
			rect.y += y;
			surface_draw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyRecall]], rect.x, rect.y);
			rect.y += y;
			surface_draw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyMode]], rect.x, rect.y);

			rect.y -= 4 * y;
			rect.x += 18;

			strDraw(e->io.screen, &e->io.normalFont, "Move with D-Pad", rect.x - 18, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Toggle tile", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Play move", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Recall tiles", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Place mode", rect.x, rect.y);
		} else {
			/* current is transMovePlace[*] */
			int y;
			SDL_Rect rect;
			coor_t *idx, *pos;
			
			idx  = &e->gui.gameGui.boardWidget.index;
			pos = &e->gui.gameGui.boardWidget.pos;

			rect.w = (e->io.normalFont.width + e->io.normalFont.spacing) * 16 + 6;
			rect.h = e->io.normalFont.height * 5 + 6;

			rect.x = pos->x;
			if (idx->x < BOARD_X / 2) {
				rect.x += BOARD_X * TILE_WIDTH;
				rect.x -= rect.w; 
			}

			rect.y = pos->y;
			if (idx->y < BOARD_Y / 2) {
				rect.y += BOARD_Y * TILE_HEIGHT;
				rect.y -= rect.h;
			} 

			rect.x -= 2;
			rect.y -= 2;
			rect.w += 4;
			rect.h += 4;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x20, 0x20, 0x70, 255));

			rect.x += 1;
			rect.y += 1;
			rect.w -= 2;
			rect.h -= 2;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x50, 0x50, 0xa0, 255));

			rect.x += 1;
			rect.y += 1;
			rect.w -= 2;
			rect.h -= 2;
			SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x00, 0x00, 0x00, 255));

			y = e->io.normalFont.height + 1;
			rect.x += 5;

			surface_draw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyPlay]], rect.x, rect.y);
			rect.y += y;
			surface_draw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeySelect]], rect.x, rect.y);
			rect.y += y;
			surface_draw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyRecall]], rect.x, rect.y);
			rect.y += y;
			surface_draw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyCancel]], rect.x, rect.y);
			rect.y += y;
			surface_draw(e->io.screen, e->io.btn[e->io.controls.game.key[gameKeyMode]], rect.x, rect.y);

			rect.y -= 4 * y;
			rect.x += 18;

			strDraw(e->io.screen, &e->io.normalFont, "Play move", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Place tile", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Recall tiles", rect.x, rect.y);
			rect.y += y; 
			strDraw(e->io.screen, &e->io.normalFont, "Cancel tile", rect.x, rect.y);
			rect.y += y;
			strDraw(e->io.screen, &e->io.normalFont, "Discard mode", rect.x, rect.y);
		}
	}	
}

void draw_guiFocusGameMenu(Env *e)
{
	NOT(e);

	guiDraw(&e->io, &e->gui, &e->game, &e->gui.transMove, &e->io.controls.game); 
	surface_draw(e->io.screen, e->io.menuBg, 0, 0); 
	drawMenuView(e->io.screen, &e->io.gameMenuMV);

	surface_draw(e->io.screen, e->io.pauseTitle, 0, 0);
	/*strDraw(e->io.screen, &e->io.normalFont, "- Pause -", SCREEN_WIDTH / 2 - 24, 18);*/
}

void draw_guiFocusPlayMenu(Env *e)
{
	NOT(e);

	drawMenuBackground(&e->io);
	drawMenuView(e->io.screen, &e->io.playMenuMV);
	surface_draw(e->io.screen, e->io.chooseGameTitle, 0, 0);
}

void draw_guiFocusOptions(Env *e)
{
	char str[64];
	int i;
	Font *f;
	MenuView *mv;
	bool lower, higher;

	NOT(e);

	lower = false;
	higher = false;

	mv = &e->io.optionsMV;

	drawMenuBackground(&e->io);
	surface_draw(e->io.screen, e->io.optionsTitle, 0, 0);
	drawMenuViewRight(e->io.screen, &e->io.optionsMV);

	for (i = 0; i < optionsFocusCount; i++) {
		bool focused = i == e->gui.options.menu.focus;
		f = focused ? &e->io.highlightFont : &e->io.normalFont;

		str[0] = '\0';
		switch (i) {
		case optionsFocusAI: {
			lower = e->gui.options.ai > 1;
			higher = e->gui.options.ai < 10;
			sprintf(str, "%d", e->gui.options.ai);
			break;
		}
		default: break;
		}
		strDraw(e->io.screen, f, str, mv->pos.x + mv->distance + 16, mv->pos.y + i * mv->spacing.y);
		if (focused) {
			int ii = interval(e->io.time, 0.2f);
			if (lower) {
				strDraw(e->io.screen, f, "<", -ii + mv->pos.x + mv->distance + 8, mv->pos.y + i * mv->spacing.y);
			}
			if (higher) {
				strDraw(e->io.screen, f, ">", ii + mv->pos.x + mv->distance + 18 + strlen(str) * (f->width + f->spacing), mv->pos.y + i * mv->spacing.y);
			}
		}
	}
}

void draw_guiGameAIPause(Env *e)
{
	int i, j;
	const char *text = "PLAYER 2...";
	NOT(e);
		
	/*draw_guiFocusGameGUI(e);*/
	surface_draw(e->io.screen, e->io.gmBack, 0, 0);
	guiDrawBoardWithoutTransMove(&e->io, &e->gui.gameGui.boardWidget, &e->game, &e->gui.gameGui.lastMove);
	drawScoreBoard(&e->gui.scoreBoard, &e->io);
	
	drawFader(&e->io, 196);
	strDraw(e->io.screen, &e->io.normalFont, text, (SCREEN_WIDTH - strlen(text) * (e->io.normalFont.width + e->io.normalFont.spacing)) / 2, 80);
	
	for (i = 0, j = e->gui.gameGui.textLog.head; i < e->gui.gameGui.textLog.size; i++, j++, j %= e->gui.gameGui.textLog.size) {
		strDraw(e->io.screen, &e->io.blackFont, e->gui.gameGui.textLog.line[j], 16, 84 + 24 + 12 * i);
	}

	drawProgressBar(e->io.screen, e->game.player[e->game.turn].aiShare.loading, 60, 100, 200, 10, 1);
}

void draw_guiFocusGameHotseatPause(Env *e)
{
	int i, j;
	char text[32];

	NOT(e);


	surface_draw(e->io.screen, e->io.gmBack, 0, 0);

	guiDrawBoardWithoutTransMove(&e->io, &e->gui.gameGui.boardWidget, &e->game, &e->gui.gameGui.lastMove);
	drawScoreBoard(&e->gui.scoreBoard, &e->io);
	
	for (i = 0, j = e->gui.gameGui.textLog.head; i < e->gui.gameGui.textLog.size; i++, j++, j %= e->gui.gameGui.textLog.size) {
		strDraw(e->io.screen, &e->io.blackFont, e->gui.gameGui.textLog.line[j], 16, 84 + 24 + 12 * i);
	}

	drawFader(&e->io, 196);

	sprintf(text, "PLAYER %d's turn", e->game.turn+1);
	strDraw(e->io.screen, &e->io.normalFont, text, (SCREEN_WIDTH - (strlen(text) * (e->io.normalFont.width + e->io.normalFont.spacing))) / 2, 80);
	strDraw(e->io.screen, &e->io.normalFont, "Press Start", (SCREEN_WIDTH - (strlen("Press Start") * (e->io.normalFont.width + e->io.normalFont.spacing))) / 2, 100);
	

}

void draw_guiFocusGameOver(Env *e)
{
	int i, j;

	NOT(e);

	drawMenuBackground(&e->io);
	surface_draw(e->io.screen, e->io.gmBack, 0, 0);
	guiDrawBoardWithoutTransMove(&e->io, &e->gui.gameGui.boardWidget, &e->game, &e->gui.gameGui.lastMove);
	drawScoreBoard(&e->gui.scoreBoard, &e->io);
	
	for (i = 0, j = e->gui.gameGui.textLog.head; i < e->gui.gameGui.textLog.size; i++, j++, j %= e->gui.gameGui.textLog.size) {
		strDraw(e->io.screen, &e->io.blackFont, e->gui.gameGui.textLog.line[j], 16, 84 + 24 + 12 * i);
	}

	drawFader(&e->io, 196);
	
	{
		SDL_Rect rect;
		rect.x = 80;
		rect.y = 68;
		rect.w = SCREEN_WIDTH - rect.x * 2;
		rect.h = 122;

		SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x20, 0x20, 0x70, 0xff));

		rect.x++;
		rect.y++;
		rect.w-=2;
		rect.h-=2;
		SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x50, 0x50, 0xa0, 0xff));

		rect.w-= 2;
		rect.h-= 2;
		rect.x++;
		rect.y++;
		SDL_FillRect(e->io.screen, &rect, SDL_MapRGBA(e->io.screen->format, 0x00, 0x00, 0x00, 0xff));
	}

	surface_draw(e->io.screen, e->io.gameOverTitle, 0, 0);

	{
		char text[64];
		sprintf(text, "PLAYER %d WON!", find_winner(&e->game) + 1);
		strDraw(e->io.screen, &e->io.highlightFont, text, (SCREEN_WIDTH - (strlen(text) * (e->io.normalFont.width + e->io.normalFont.spacing))) / 2, 80);
	}

	for (i = 0; i < e->game.playerNum; i++) {
		char text[64];
		Font *f = (find_winner(&e->game) == i) ? &e->io.highlightFont : &e->io.normalFont;
		sprintf(text, "PLAYER %d:     %d", i + 1, e->game.player[i].score);
		strDraw(e->io.screen, f, text,
			(SCREEN_WIDTH - (15 * (f->width + f->spacing))) / 2 - 12/2, 
			14 * i + 120);
	}
}

void draw_guiFocusGameAreYouSureQuit(Env *e)
{
/*
	const int orgX = (SCREEN_WIDTH - e->io.areYouSureQuit->w)/2;
	const int orgY = 80;
*/
	surface_draw(e->io.screen, e->io.menuBg, 0, 0);
	drawMenuView(e->io.screen, &e->io.yesNoMV);
	surface_draw(e->io.screen, e->io.areYouSureTitle, 0, 0);
}

void draw_nothing(Env *e)
{
	NOT(e);
	/* nothing */
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;
    default:
        return 0;
    }
}

void pixelate(SDL_Surface *s, int size)
{
	int i, j, k, x, y, w, h;
	Uint32 c[4], p, color;
	SDL_Rect rect;

	NOT(s);

	if (size <= 0) {
		return;
	}

	for (x = 0; x < s->w; x += size) {
		for (y = 0; y < s->h; y += size) {
			for (k = 0; k < 4; k++) {
				c[k] = 0;
			}
			w = (size + x < s->w) ? size : size + x - s->w;
			h = (size + y < s->h) ? size : size + y - s->h;
			
			if (w == 0 || h == 0 || w < size || h < size) {
				continue;
			}
			for (i = 0; i < w; i++) {
				for (j = 0; j < h; j++) {
					p = getpixel(s, i + x, j + y);
					for (k = 0; k < 4; k++) {
						c[k] += (p >> k * 8) & 0xff;
					}
				}
			}
			color = 0;
			for (k = 0; k < 4; k++) {
				c[k] /= size * size;
				c[k] &= 0xff;
				color |= (c[k] << k * 8);
			}
			rect.x = x;
			rect.y = y;
			rect.w = w;
			rect.h = h;
			SDL_FillRect(s, &rect, color);
		}
	}
}

void draw_guiFocusTransScreen(Env *e)
{
	void (*func)(Env *e);
	GUI *g;
	TransScreen *ts;
	float percent;

	NOT(e);

	g = &e->gui;
	ts = &g->transScreen;
	percent = ts->elapsed / ts->time;
	percent = percent > 1.f ? 1.f : (percent < 0.f ? 0.f : percent);

	switch (percent < 0.5f ? ts->focus : ts->next) {
	case guiFocusTitle: func = draw_guiFocusTitle; break;
	case guiFocusMenu: func = draw_guiFocusMenu; break;
	case guiFocusRules: func = draw_guiFocusRules; break;
	case guiFocusSettings: func = draw_guiFocusSettings; break;
	case guiFocusControls: func = draw_guiFocusControls; break;
	case guiFocusGameGUI: func = draw_guiFocusGameGUI; break;
	case guiFocusGameMenu: func = draw_guiFocusGameMenu; break;
	case guiFocusPlayMenu: func = draw_guiFocusPlayMenu; break;
	case guiFocusOptions: func = draw_guiFocusOptions; break;
	case guiFocusGameAIPause: func = draw_guiGameAIPause; break;
	case guiFocusGameHotseatPause: func = draw_guiFocusGameHotseatPause; break;
	case guiFocusGameOver: func = draw_guiFocusGameOver; break;
	case guiFocusGameAreYouSureQuit: func = draw_guiFocusGameAreYouSureQuit; break;
	default: func = draw_nothing; break;
	}

	func(e);
	
	percent = percent > 1.f ? 1.f : (percent < 0.f ? 0.f : percent);
	switch (ts->type) {
	case transScreenFadeBlack: {
		int val;
		assert(percent >= 0.f && percent <= 1.f);
		val = percent < 0.5f ? (percent / 0.5f) * 255 : 255 - ((percent - 0.5f) / 0.5f) * 255;
		drawFader(&e->io, val);
		break;
	}
	case transScreenFadePause: {
		int val;
		assert(percent >= 0.f && percent <= 1.f);
		val = 255;
		if (percent < 0.33f) {
			val = (percent / 0.33)  * 255;
		}
		if (percent >= 0.66f) {
			val = 255 - ((percent - 0.66f) / 0.34)  * 255;
		}
		val = val < 0 ? 0 : val;
		val = val > 255 ? 255 : val;
		drawFader(&e->io, val);
		break;
	}
	case transScreenFadePausePixelate: {
		int val;
		assert(percent >= 0.f && percent <= 1.f);
		val = 255;
		if (percent < 0.33f) {
			val = (percent / 0.33)  * 255;
		}
		if (percent >= 0.66f) {
			val = 255 - ((percent - 0.66f) / 0.34)  * 255;
		}
		val = val < 0 ? 0 : val;
		val = val > 255 ? 255 : val;
		drawFader(&e->io, val);
		pixelate(e->io.screen, val / 15);
		break;
	}
	default: break;
	}
}

void draw(Env *e)
{
	NOT(e);

	SDL_FillRect(e->io.screen, NULL, 0);
	switch (e->gui.focus) {
	case guiFocusTitle: draw_guiFocusTitle(e); break;
	case guiFocusMenu: draw_guiFocusMenu(e); break;
	case guiFocusRules: draw_guiFocusRules(e); break;
	case guiFocusSettings: draw_guiFocusSettings(e); break;
	case guiFocusControls: draw_guiFocusControls(e); break;
	case guiFocusGameGUI: draw_guiFocusGameGUI(e); break;
	case guiFocusGameMenu: draw_guiFocusGameMenu(e); break;
	case guiFocusPlayMenu: draw_guiFocusPlayMenu(e); break;
	case guiFocusOptions: draw_guiFocusOptions(e); break;
	case guiFocusGameAIPause: draw_guiGameAIPause(e); break;
	case guiFocusGameHotseatPause: draw_guiFocusGameHotseatPause(e); break;
	case guiFocusGameOver: draw_guiFocusGameOver(e); break;
	case guiFocusGameAreYouSureQuit: draw_guiFocusGameAreYouSureQuit(e); break;
	case guiFocusTransScreen: draw_guiFocusTransScreen(e); break;
	default: break;
	}
	SDL_Flip(e->io.screen);
}

