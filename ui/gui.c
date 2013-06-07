#include "gui.h"
#include "init.h"


#define TILE_SPACING_X	2
#define TILE_SPACING_Y	0


void initBoardWidget(struct boardWidget *bw)
{
	int y, x;

	NOT(bw);
	
	bw->focus.x = 0;
	bw->focus.y = 0;

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {	
			bw->locWidget[y][x].tile.type = TILE_NONE;
		}
	}
}


void guiInit(struct gui *g)
{
	NOT(g);
	g->gameWidget.focus = FOCUS_BOARD;
	initBoardWidget(&g->gameWidget.boardWidget);
}


bool fontmapInit(struct font *f, int w, int h, const char *filename)
{
	NOT(f), NOT(filename);

	f->width = w;
	f->height = h;
	f->map = surfaceLoad(filename);
	return f->map != NULL;
}


void fontmapQuit(struct font *f)
{
	NOT(f), NOT(f->map);

	surfaceFree(f->map);
}


void strDraw(SDL_Surface *s, struct font *f, const char *str, int x, int y)
{
	int i;
	char c;
	SDL_Rect offset, clip;

	NOT(s), NOT(f), NOT(f->map), NOT(str);

	offset.x = x;
	offset.y = y;
	clip.y = 0;
	clip.h = f->height;
	clip.w = f->width;
	for (i = 0; str[i] != '\0'; i++) {
		c = str[i];
		/* [32..126] are drawable ASCII chars */
		if (c >= 32 && c <= 126) {
			clip.x = f->width * (c - 32);
			SDL_BlitSurface(f->map, &clip, s, &offset);
		}
		offset.x += f->width;
	}
}


bool ioInit(struct io *io)
{
	NOT(io);

	return false;
}


void keystateInit(struct keystate *ks)
{
	NOT(ks);
	
	ks->type = KEYSTATE_UNTOUCHED;
	ks->time = 0.0f;
}


void controlsInit(struct controls *c)
{
	NOT(c);
	
	keystateInit(&c->up);
	keystateInit(&c->down);
	keystateInit(&c->right);
	keystateInit(&c->left);
	keystateInit(&c->a);
	keystateInit(&c->b);
	keystateInit(&c->x);
	keystateInit(&c->y);
}


bool init(struct env *e)
{
	int i;
	char str[32];
	SDL_Surface *tile;

	NOT(e);

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return false;
	}
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("finite", NULL);
	e->io.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
			SDL_SWSURFACE);

	if (e->io.screen == NULL) {
		return false;
	}

	if (!dictInit(&e->game.dict, RES_PATH "dict.txt")) {
		return false;
	}
	
	e->io.back = surfaceLoad(RES_PATH "back.png");
	if (!e->io.back) {
		return false;
	}

	e->io.lockon = surfaceLoad(RES_PATH "lockon.png");
	if (!e->io.lockon) {
		return false;
	}

	if (!fontmapInit(&e->io.white_font, 6, 12, RES_PATH "white_font.png")) {
		return false;
	}

	if (!fontmapInit(&e->io.black_font, 6, 12, RES_PATH "black_font.png")) {
		return false;
	}
	
	tile = surfaceLoad(RES_PATH "tile.png");
	if (!tile) {
		return false;
	}
	e->io.wild = surfaceCpy(tile);
	for (i = 0; i < LETTER_COUNT; i++) {
		e->io.tile[TILE_WILD][i] = surfaceCpy(tile);
		if (!e->io.tile[TILE_WILD][i]) {
			return false;
		}
		e->io.tile[TILE_LETTER][i] = surfaceCpy(tile);
		if (!e->io.tile[TILE_LETTER][i]) {
			return false;
		}
		sprintf(str,"%c", i + 'a');
		strDraw(e->io.tile[TILE_WILD][i], &e->io.black_font, str, 3, 0);
		sprintf(str,"%c", i + 'A');
		strDraw(e->io.tile[TILE_LETTER][i], &e->io.black_font, str, 3, 0);
	}
	surfaceFree(tile);
	boardInit(&e->game.board);
	bagInit(&e->game.bag);
	guiInit(&e->gui);
	playerInit(&e->game.player[0], &e->game.bag);
	controlsInit(&e->controls);
	guiInit(&e->gui);
	return true;
}


void quit(struct env *e)
{
	int i;

	NOT(e);

	dictQuit(&e->game.dict);
	surfaceFree(e->io.screen);
	surfaceFree(e->io.back);
	surfaceFree(e->io.lockon);
	surfaceFree(e->io.wild);
	for (i = 0; i < LETTER_COUNT; i++) {
		surfaceFree(e->io.tile[TILE_WILD][i]);
		surfaceFree(e->io.tile[TILE_LETTER][i]);
	}
	fontmapQuit(&e->io.white_font);
	fontmapQuit(&e->io.black_font);
	SDL_Quit();
}


void keystateUpdate(struct keystate *ks, bool touched)
{
	NOT(ks);

	if (touched) {
		switch(ks->type) {
		case KEYSTATE_UNTOUCHED: {
			ks->type = KEYSTATE_PRESSED;
			ks->time = 0.0f;
			break;
		}
		case KEYSTATE_PRESSED: {
			ks->type = KEYSTATE_HELD;
			ks->time = 0.0f;
			break;
		}
		case KEYSTATE_HELD: {
			ks->time += 1.0f / 60.0f;
			break;
		}
		case KEYSTATE_RELEASED: {
			ks->type = KEYSTATE_PRESSED;
			ks->time = 0.0f;
			break;
		}
		default: break;
		}
	} else {
		switch(ks->type) {
		case KEYSTATE_UNTOUCHED: {
			ks->time += 1.0f / 60.0f;
			break;
		}
		case KEYSTATE_PRESSED: {
			ks->type = KEYSTATE_RELEASED;
			ks->time = 0.0f;
			break;
		}
		case KEYSTATE_HELD: {
			ks->type = KEYSTATE_RELEASED;
			ks->time = 0.0f;
			break;
		}
		case KEYSTATE_RELEASED: {
			ks->type = KEYSTATE_UNTOUCHED;
			ks->time = 0.0f;
			break;
		}
		default: break;
		}
	}
}


bool handleEvent(struct controls *c)
{
	SDL_Event event;
	Uint8 *ks;

	NOT(c);

	ks = SDL_GetKeyState(NULL);
	NOT(ks);
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: return true;
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				return true;
			}
			ks[event.key.keysym.sym] = true;
			break;
		}
		case SDL_KEYUP: ks[event.key.keysym.sym] = false; break;
		default: break;
		}
	}
	keystateUpdate(&c->up,    ks[SDLK_UP]);
	keystateUpdate(&c->down,  ks[SDLK_DOWN]);
	keystateUpdate(&c->left,  ks[SDLK_LEFT]);
	keystateUpdate(&c->right, ks[SDLK_RIGHT]);
	keystateUpdate(&c->a, ks[SDLK_a]);
	keystateUpdate(&c->b, ks[SDLK_b]);
	keystateUpdate(&c->x, ks[SDLK_x]);
	keystateUpdate(&c->y, ks[SDLK_y]);
	return false;
}

/*
bool solid_move(struct keystate *ks)
{
	NOT(ks);

	return ks->type == KEYSTATE_PRESSED ||
			(ks->type == KEYSTATE_HELD && ks->time >= 0.3f);
}
*/


void rackWidgetSyncControls(struct gameWidget *gw, struct controls *c)
{
	struct rackWidget *rw;

	NOT(gw), NOT(c);
	assert(gw->focus == FOCUS_RACK);

	rw = &gw->rackWidget;

	if (c->up.type == KEYSTATE_PRESSED) {
		int x = rw->focus + 7;
		gw->focus = FOCUS_BOARD;
		gw->boardWidget.focus.y = BOARD_Y - 1;
		gw->boardWidget.focus.x = x;
		return;
	}
	if (c->left.type == KEYSTATE_PRESSED) {
		rw->focus--;
	}
	if (c->right.type == KEYSTATE_PRESSED) {
		rw->focus++;
	}
	if (rw->focus < 0) {
		gw->focus = FOCUS_CHOICE;
		gw->choiceWidget.focus = CHOICE_COUNT - 1;
		rw->focus = 0;
		return;
	}
	if (rw->focus >= RACK_SIZE) {
		rw->focus = RACK_SIZE - 1;
		return;
	}
}


void choiceWidgetSyncControls(struct gameWidget *gw, struct controls *c)
{
	struct choiceWidget *cw;

	NOT(gw), NOT(c);
	assert(gw->focus == FOCUS_CHOICE);

	cw = &gw->choiceWidget;

	if (c->up.type == KEYSTATE_PRESSED) {
		int x = cw->focus + 1;	/* 1 is an offset, must change */
		gw->focus = FOCUS_BOARD;
		gw->boardWidget.focus.y = BOARD_Y - 1;
		gw->boardWidget.focus.x = x;
		return;
	}
	if (c->left.type == KEYSTATE_PRESSED) {
		cw->focus--;
	}
	if (c->right.type == KEYSTATE_PRESSED) {
		cw->focus++;
	}
	if (cw->focus < 0) {
		cw->focus = 0;
		return;
	}
	if (cw->focus >= CHOICE_COUNT) {
		gw->focus = FOCUS_RACK;
		gw->rackWidget.focus = 0;
		cw->focus = CHOICE_COUNT - 1;
		return;
	}
}


void gameWidgetSyncGame(struct gameWidget *gw, struct game *g)
{
	int y, x, i;
	struct boardWidget *bw;
	struct rackWidget *rw;

	NOT(gw), NOT(g);
	
	bw = &gw->boardWidget;

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			bw->locWidget[y][x].sq = g->board.sq[y][x];
			if (g->board.tile[y][x].type == TILE_NONE) {
				bw->locWidget[y][x].enabled = true;
				bw->locWidget[y][x].tile.type = TILE_NONE;
			} else {
				bw->locWidget[y][x].enabled = false;
				bw->locWidget[y][x].tile = g->board.tile[y][x];
			}
		}
	}

	rw = &gw->rackWidget;

	for (i = 0; i < RACK_SIZE; i++) {
		rw->tileWidget[i].tile.type = g->player[g->turn].tile[i].type;
		rw->tileWidget[i].tile.letter = g->player[g->turn].tile[i].letter;
		rw->tileWidget[i].enabled = rw->tileWidget[i].tile.type == TILE_NONE;
	}
}

void boardWidgetSyncControls(struct gameWidget *gw, struct controls *c)
{	
	struct boardWidget *bw;
	NOT(gw), NOT(c);
	assert(gw->focus == FOCUS_BOARD);

	bw = &gw->boardWidget;

	bw->select = c->a.type == KEYSTATE_PRESSED;
	bw->cancel = c->b.type == KEYSTATE_PRESSED;
	
	if (c->up.type == KEYSTATE_PRESSED) {
		bw->focus.y--;
	}
	if (c->down.type == KEYSTATE_PRESSED) {
		bw->focus.y++;
	}
	if (c->left.type == KEYSTATE_PRESSED) {
		bw->focus.x--;
		bw->focus.x += BOARD_X;
		bw->focus.x %= BOARD_X;
	}
	if (c->right.type == KEYSTATE_PRESSED) {
		bw->focus.x++;
		bw->focus.x %= BOARD_X;
	}
	if (bw->focus.y < 0) {
		bw->focus.y = 0;
	}
	if (bw->focus.y >= BOARD_Y) {
		if (bw->focus.x <=  5) {
			int choice = bw->focus.x - (0+1);
			gw->focus = FOCUS_CHOICE;
			if (choice < 0) {
				choice = 0;
			}
			if (choice >= CHOICE_COUNT) {
				choice = CHOICE_COUNT - 1;
			}
			gw->choiceWidget.focus = choice;
		}
		if (bw->focus.x >= 6) {
			int rack = bw->focus.x - (6 + 1);
			gw->focus = FOCUS_RACK;
			if (rack < 0) {
				rack = 0;
			}
			if (rack >= RACK_SIZE) {
				rack = RACK_SIZE - 1;
			}
			gw->rackWidget.focus = rack;
		}
		bw->focus.y = BOARD_Y - 1;
	}
}


void gameWidgetSyncControls(struct gameWidget *gw, struct controls *c)
{
	switch (gw->focus) {
	case FOCUS_BOARD: {
		boardWidgetSyncControls(gw, c);
		break;
	}
	case FOCUS_RACK: {
		rackWidgetSyncControls(gw, c);
		break;
	}
	case FOCUS_CHOICE: {
		choiceWidgetSyncControls(gw, c);
		break;
	}
	default: break;
	}
}


bool validRackId(int id)
{
	return id >= 0 && id < RACK_SIZE;
}


bool validGridId(struct coor c)
{
	return c.x >= 0 && c.y >= 0 && c.x < BOARD_X && c.y < BOARD_Y;
}


bool isSwappable(int rackId, struct rackWidget *rw)
{
	return		rackId != rw->focus &&
			rw->tileWidget[rw->focus].enabled &&
			rw->tileWidget[rw->focus].tile.type != TILE_NONE;
}

void fdChoiceCmd(struct cmd *c, struct choiceWidget *cw)
{

	NOT(c), NOT(cw);

	switch (cw->focus) {
	case CHOICE_MODE:   c->type = CMD_MODE;   break;
	case CHOICE_PLAY:   c->type = CMD_PLAY;   break;
	case CHOICE_RECALL: c->type = CMD_RECALL; break;
	case CHOICE_QUIT:   c->type = CMD_QUIT;   break;
	default: break;
	}
}


void gameWidgetToCmdMovePlace(struct cmd *c, struct gameWidget *gw, struct transMovePlace *tmp)
{
	struct boardWidget *bw;
	struct rackWidget *rw;
	struct choiceWidget *cw;

	NOT(c), NOT(gw), NOT(tmp);
		
	bw = &gw->boardWidget;
	rw = &gw->rackWidget;
	cw = &gw->choiceWidget;

	c->focus = gw->focus;
	c->type = CMD_INVALID;
	switch (gw->focus) {
	case FOCUS_BOARD: {
		c->data.board = bw->focus;	
		if (tmp->transTile.has) {
			if (bw->select) {
				c->type = CMD_PLACE;
			} else if (bw->cancel) {
				c->type = CMD_DROP;
			} else {
				c->focus = FOCUS_INVALID;
			}
		} else {
			c->focus = FOCUS_INVALID;
			if (bw->select && validRackId(tmp->rackId[bw->focus.y][bw->focus.x])) {
				c->type = CMD_GRAB;
			} else {
				c->focus = FOCUS_INVALID;
			}
		}
		break;
	}
	case FOCUS_RACK: {
		c->data.rack = rw->focus;
		if (tmp->transTile.has) {
			if (rw->select && isSwappable(tmp->transTile.rackId, rw)) {
				/* c->type = CMD_SWAP; */
				c->focus = FOCUS_INVALID;
			} else if (rw->cancel) {
				c->type = CMD_DROP;
			} else {
				c->focus = FOCUS_INVALID;
			}
		} else {
			if (rw->select && !validGridId(tmp->gridId[rw->focus])) {
				c->type = CMD_GRAB;
			} else {
				c->focus = FOCUS_INVALID;
			}
		}
		break;
	}
	case FOCUS_CHOICE: {
		c->data.choice = cw->focus;
		if (tmp->transTile.has) {
			if (cw->select) {
				fdChoiceCmd(c, cw);
			} else if (cw->cancel) {
				c->type = CMD_DROP;
			} else {
				c->focus = FOCUS_INVALID;
			}
		} else {
			if (cw->select) {
				fdChoiceCmd(c, cw);
			} else {
				c->focus = FOCUS_INVALID;
			}
		}
		break;
	}
	default: break;
	}
}


void gameWidgetToCmdMoveDiscard(struct cmd *c, struct gameWidget *gw, struct transMoveDiscard *tmd)
{
	struct rackWidget *rw;
	struct choiceWidget *cw;

	NOT(c), NOT(gw), NOT(tmd);

	rw = &gw->rackWidget;
	cw = &gw->choiceWidget;

	c->focus = gw->focus;
	c->type = CMD_INVALID;
	switch (gw->focus) {
	case FOCUS_RACK: {
		c->data.rack = rw->focus;
		if (!tmd->tile[rw->focus]) {
			if (rw->select || rw->cancel) {
				c->type = CMD_DISCARD;
			} else {
				c->focus = FOCUS_INVALID;
			}
		} else {
			if (rw->select) {
				c->type = CMD_KEEP;
			} else {
				c->focus = FOCUS_INVALID;
			}
		}
		break;
	}
	case FOCUS_CHOICE: {
		c->data.choice = cw->focus;
		if (cw->select) {
			fdChoiceCmd(c, cw);
		} else {
			c->focus = FOCUS_INVALID;
		}
		break;
	}
	default: {
		c->type = CMD_INVALID;
		c->focus = FOCUS_INVALID;
		break;
	}
	}
}


void gameWidgetToCmd(struct cmd *c, struct gameWidget *gw, struct transMove *tm)
{
	NOT(c), NOT(gw), NOT(tm);

	switch (tm->type) {
	case TRANS_MOVE_PLACE: {
		gameWidgetToCmdMovePlace(c, gw, &tm->data.place);
		break;
	}
	case TRANS_MOVE_DISCARD: {
		gameWidgetToCmdMoveDiscard(c, gw, &tm->data.discard);
		break;
	}
	default: break;
	}
}


void transMoveToMove(struct move *m, struct transMove *tm)
{
	int i;
	
	NOT(m), NOT(tm);

	moveClr(m);
	m->player_id = tm->playerId;
	switch (tm->type) {
	case TRANS_MOVE_PLACE: {
		m->type = MOVE_PLACE;
		assert(tm->data.place.num > 0 && tm->data.place.num < RACK_SIZE);
		for (i = 0; i < tm->data.place.num; i++) {
			struct coor coor = tm->data.place.gridId[i];
			m->data.place.coor[i] = coor;
			m->data.place.rack_id[i] = tm->data.place.rackId[coor.y][coor.x];
		}
		break;
	}
	case TRANS_MOVE_DISCARD: {
		m->type = MOVE_DISCARD;
		m->data.discard.num = 0;
		for (i = 0; i < RACK_SIZE; i++) {
			if (tm->data.discard.tile[i]) {
				m->data.discard.rack_id[m->data.discard.num] = i;
				m->data.discard.num++;
			}
		}
		break;
	}
	case TRANS_MOVE_SKIP: {
		m->type = MOVE_SKIP;
		break;
	}
	case TRANS_MOVE_QUIT: {
		m->type = MOVE_QUIT;
		break;
	}
	case TRANS_MOVE_NONE:
	case TRANS_MOVE_INVALID:
	default: {
		m->type = MOVE_INVALID;
		break;
	}
	}
}


void cmdIntoTransMovePlace(struct transMove *tm, struct cmd *c)
{
	const struct coor invalidCoor = {-1, -1};
	const int invalidId = -1;
	struct transMovePlace *tmp;

	NOT(tm), NOT(c), assert(tm->type == TRANS_MOVE_PLACE);

	tmp = &tm->data.place;

	switch (c->focus) {
	case FOCUS_BOARD: {
		switch (c->type) {
		case CMD_DROP: {
			tmp->transTile.has = false;
			break;
		}
		case CMD_PLACE: {
			tmp->transTile.has = false;
			tmp->rackId[c->data.board.y][c->data.board.x] = tmp->transTile.rackId;
			tmp->gridId[tmp->transTile.rackId] = c->data.board;
			tmp->num++;
			break;
		}
		case CMD_GRAB: {
			tmp->transTile.has = true;
			tmp->transTile.rackId = tmp->rackId[c->data.board.y][c->data.board.x];
			tmp->rackId[c->data.board.y][c->data.board.x] = invalidId;
			tmp->gridId[tmp->transTile.rackId] = invalidCoor;
			tmp->num--;
			break;
		}
		default: break;
		}
		break;
	}
	case FOCUS_RACK: {
		switch (c->type) {
		case CMD_DROP: {
			tmp->transTile.has = false;
			break;
		}
		case CMD_GRAB: {
			tmp->transTile.has = true;
			tmp->transTile.rackId = c->data.rack;
			break;
		}
		/*
		case CMD_SWAP: {
			tmp->transTile.has = true;
			break;
		}
		*/
		default: break;
		}
		break;
	}
	case FOCUS_CHOICE: {
		switch (c->type) {
		case CMD_MODE: {
			/* clear tmp */
			tm->type = TRANS_MOVE_DISCARD;
			break;
		}
		case CMD_PLAY: {
			tm->act = true;
			break;
		}
		case CMD_RECALL: {
			/* clear tmp */
			break;
		}
		case CMD_QUIT: {
			tm->type = TRANS_MOVE_QUIT;
			tm->act = true;
			break;
		}
		default: break;
		}
		break;
	}
	default: break;
	}
}


void cmdIntoTransMoveDiscard(struct transMove *tm, struct cmd *c)
{
	struct transMoveDiscard *tmd;
	
	NOT(tm), NOT(c), assert(tm->type == TRANS_MOVE_DISCARD);

	tmd = &tm->data.discard;
	
	switch (c->focus) {
	case FOCUS_RACK: {
		switch (c->type) {
		case CMD_DISCARD: {
			tmd->tile[c->data.rack] = true;
			break;
		}
		case CMD_KEEP: {
			tmd->tile[c->data.rack] = false;
			break;
		}
		default: break;
		}
		break;
	}
	case FOCUS_CHOICE: {
		switch (c->type) {
		case CMD_MODE:   break;
		case CMD_PLAY:   break;
		case CMD_RECALL: break;
		case CMD_QUIT:   break;
		default: break;
		}
		break;
	}
	default: break;
	}
}


void cmdIntoTransMove(struct transMove *tm, struct cmd *c)
{
	
	NOT(tm), NOT(c);
	
	switch (tm->type) {
	case TRANS_MOVE_PLACE: {
		cmdIntoTransMovePlace(tm, c);
		break;
	}
	case TRANS_MOVE_DISCARD: {
		cmdIntoTransMoveDiscard(tm, c);
		break;
	}
	case TRANS_MOVE_NONE:
	case TRANS_MOVE_INVALID:
	default: break;
	}
}


void gameSyncGameWidget(struct game *g, struct gameWidget *gw)
{
	NOT(g), NOT(gw);
}


void guiSyncGame(struct gui *gu, struct game *ga)
{
	NOT(gu), NOT(ga);
	
	gameWidgetSyncGame(&gu->gameWidget, ga);
}


void guiSyncControls(struct gui *g, struct controls *c)
{
	NOT(g), NOT(c);
	
	gameWidgetSyncControls(&g->gameWidget, c);
}


void gameSyncGui(struct game *ga, struct gui *gu)
{
	NOT(ga), NOT(gu);
	
	gameSyncGameWidget(ga, &gu->gameWidget);
}


void update(struct env *e)
{
	/*
	 * Follows a MVC pattern:
	 * Game           -> Model
	 * GUI + IO       -> View
	 * GUI + Controls -> Controller
	 */
	NOT(e);

	guiSyncGame(&e->gui, &e->game);
	guiSyncControls(&e->gui, &e->controls);
	gameSyncGui(&e->game, &e->gui);
}


void boardWidgetDraw(struct io *io, struct boardWidget *bw)
{
	int xOffset, yOffset, x, y, w, h, letter, type;
	
	NOT(io), NOT(bw);
	
	xOffset = 106;
	yOffset = 6;
	w = io->wild->w + TILE_SPACING_X;
	h = io->wild->h + TILE_SPACING_Y;
	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			type = 1;
			switch (bw->locWidget[y][x].tile.type) {
			case TILE_WILD: 
				type = 0;
			case TILE_LETTER:
				letter = (int)bw->locWidget[y][x].tile.letter;
				surfaceDraw(io->screen,
						io->tile[type][letter],
						xOffset + x * w,
					    	yOffset + y * h);
				break;
			default: break;
			}
		}
	}
}


void rackWidgetDraw(struct io *io, struct rackWidget *rw)
{
	int xOffset, yOffset, w, i, letter, type;
	SDL_Surface *t;

	NOT(io), NOT(rw), NOT(io->wild);

	xOffset = 162;
	yOffset = 222;
	w = io->wild->w + TILE_SPACING_X;
	for (i = 0; i < RACK_SIZE; i++) {
		type = rw->tileWidget[i].tile.type;
		t = io->wild;
		if (type == TILE_NONE) {
			continue;
		}
		if (type == TILE_LETTER) {
			letter = rw->tileWidget[i].tile.letter;
			assert(letter > LETTER_INVALID && letter < LETTER_COUNT);
			t = io->tile[TILE_LETTER][letter];
		}
		surfaceDraw(io->screen, t, xOffset + i * w, yOffset);
	}
	
}


void gameWidgetDraw(struct io *io, struct gameWidget *gw)
{
	int x, y;

	NOT(io), NOT(gw);

	boardWidgetDraw(io, &gw->boardWidget);
	rackWidgetDraw(io, &gw->rackWidget);

	x = 0;
	y = 0;
	switch (gw->focus) {
	case FOCUS_BOARD: {
		x = 106 + 14 * gw->boardWidget.focus.x;
		y = 6 + 14 * gw->boardWidget.focus.y;
		break;
	}
	case FOCUS_RACK: {
		x = 162 + 14 * gw->rackWidget.focus;
		y = 222;
		break;
	}
	case FOCUS_CHOICE: {
		x = 80 + 14 * gw->choiceWidget.focus;
		y = 222;
		break;
	}
	default: break;
	}
	surfaceDraw(io->screen, io->lockon, x + -2, y + -2);
}


void guiDraw(struct io *io, struct gui *g)
{
	NOT(io), NOT(g);

	gameWidgetDraw(io, &g->gameWidget);
}


void draw(struct env *e)
{
	NOT(e);

	SDL_FillRect(e->io.screen, NULL, 0);
	surfaceDraw(e->io.screen, e->io.back, 0, 0);
	guiDraw(&e->io, &e->gui);
	SDL_Flip(e->io.screen);
}


void exec(struct env *e)
{
	int st, q = 0;
	e->game.turn = 0;

	NOT(e);

	do {
		st = SDL_GetTicks();
		q = handleEvent(&e->controls);
		update(e);
		draw(e);
		delay(st, SDL_GetTicks(), 60);
	} while (!q);
}


int gui()
{
	int exit_status = EXIT_FAILURE;
	struct env e;
	if (init(&e)) {
		exec(&e);
		exit_status = EXIT_SUCCESS;
	}
	quit(&e);
	return exit_status;
}


