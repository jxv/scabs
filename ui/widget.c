#include "widget.h"


bool validRackId(int id)
{
	return id >= 0 && id < RACK_SIZE;
}


bool validGridId(struct coor c)
{
	return c.x >= 0 && c.y >= 0 && c.x < BOARD_X && c.y < BOARD_Y;
}


void mkGridWidgetByDim(struct gridWidget *gw)
{
	int i, y, x;

	NOT(gw);

	gw->index.x = 0;
	gw->index.y = 0;
	gw->button  = memAlloc(sizeof(bool*) * gw->height);
	for (i = 0; i < gw->height; i++) {
		gw->button[i] = memAlloc(sizeof(bool) * gw->width);
	}

	for (y = 0; y < gw->height; y++) {
		for (x = 0; x < gw->width; x++) {
			gw->button[y][x] = false;
			gw->button[y][x] = false;
		}
	}
}


void mkRackWidget(struct gridWidget *gw)
{
	NOT(gw);
	
	gw->width  = RACK_SIZE;
	gw->height = 1;
	mkGridWidgetByDim(gw);
}


void mkChoiceWidget(struct gridWidget *gw)
{
	NOT(gw);
	
	gw->width  = CHOICE_COUNT;
	gw->height = 1;
	mkGridWidgetByDim(gw);
}


void mkBoardWidget(struct gridWidget *gw)
{
	NOT(gw);
	
	gw->width  = BOARD_X;
	gw->height = BOARD_Y;
	mkGridWidgetByDim(gw);
}


void boardWidgetControls(struct cmd *cmd, struct gameGui *gg, struct controls *c)
{
	struct gridWidget *bw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == GUI_FOCUS_BOARD);

	bw = &gg->boardWidget;
	cmd->type = CMD_INVALID;

	if (c->x.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_PREV;
		return;
	}
	if (c->y.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_NEXT;
		return;
	}
	if (c->a.type == KEYSTATE_PRESSED && bw->button[bw->index.y][bw->index.x]) {
		cmd->type = CMD_BOARD;
		cmd->data.board = bw->index;
		return;
	}
	if (c->up.type == KEYSTATE_PRESSED) {
		bw->index.y += BOARD_Y;
		bw->index.y--;
		bw->index.y %= BOARD_Y;
		return;
	}
	if (c->down.type == KEYSTATE_PRESSED) {
		bw->index.y++;
		bw->index.y %= BOARD_Y;
		return;
	}
	if (c->left.type == KEYSTATE_PRESSED) {
		bw->index.x += BOARD_X;
		bw->index.x--;
		bw->index.x %= BOARD_X;
		return;
	}
	if (c->right.type == KEYSTATE_PRESSED) {
		bw->index.x++;
		bw->index.x %= BOARD_X;
	}
}


void choiceWidgetControls(struct cmd *cmd, struct gameGui *gg, struct controls *c)
{
	struct gridWidget *cw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == GUI_FOCUS_CHOICE);

	cw = &gg->choiceWidget;
	cw->index.y = 0;
	cmd->type = CMD_INVALID;

	if (c->x.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_PREV;
		return;
	}
	if (c->y.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_NEXT;
		return;
	}
	if (cw->button[cw->index.y][cw->index.x]) {
		if (c->a.type == KEYSTATE_PRESSED) {
			switch (cw->index.x) {
			case CHOICE_RECALL: cmd->type = CMD_RECALL; break;
			case CHOICE_PLAY: cmd->type = CMD_PLAY; break;
			case CHOICE_MODE: /* fall through */
			default: break;
			}
			return;
		}
		if (c->up.type == KEYSTATE_PRESSED) {
			switch (cw->index.x) {
			case CHOICE_MODE: cmd->type = CMD_MODE_UP; break;
			case CHOICE_RECALL: /* fall through */
			case CHOICE_PLAY:
			default: break;
			}
			return;
		}
		if (c->down.type == KEYSTATE_PRESSED) {
			switch (cw->index.x) {
			case CHOICE_MODE: cmd->type = CMD_MODE_DOWN; break;
			case CHOICE_RECALL: /* fall through */
			case CHOICE_PLAY:
			default: break;
			}
			return;
		}
	}
	if (c->left.type == KEYSTATE_PRESSED) {
		cw->index.x += CHOICE_COUNT; 
		cw->index.x--;
		cw->index.x %= CHOICE_COUNT;
		return;
	}
	if (c->right.type == KEYSTATE_PRESSED) {
		cw->index.x++;
		cw->index.x %= CHOICE_COUNT;
	}
}


void rackWidgetControls(struct cmd *cmd, struct gameGui *gg, struct controls *c)
{
	struct gridWidget *rw;

	NOT(cmd);
	NOT(gg);
	NOT(c);
	assert(gg->focus == GUI_FOCUS_RACK);
	
	rw = &gg->rackWidget;
	rw->index.y = 0;
	cmd->type = CMD_INVALID;

	if (c->x.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_PREV;
		return;
	}
	if (c->y.type == KEYSTATE_PRESSED) {
		cmd->type = CMD_FOCUS_NEXT;
		return;
	}
	if (c->a.type == KEYSTATE_PRESSED && rw->button[rw->index.y][rw->index.x]) {
		cmd->type = CMD_BOARD;
		cmd->data.rack = rw->index.x;
		return;
	}
	if (c->left.type == KEYSTATE_PRESSED) {
		rw->index.x += RACK_SIZE; 
		rw->index.x--;
		rw->index.x %= RACK_SIZE;
		return;
	}
	if (c->right.type == KEYSTATE_PRESSED) {
		rw->index.x++;
		rw->index.x %= RACK_SIZE;
	}
}


void updateBoardWidget(struct gridWidget *bw, struct transMove *tm)
{
	NOT(bw);
	NOT(tm);
	
	switch (tm->type) {
	case TRANS_MOVE_PLACE_INIT:
	case TRANS_MOVE_PLACE:
	case TRANS_MOVE_PLACE_HOLD:
	case TRANS_MOVE_DISCARD_INIT:
	case TRANS_MOVE_DISCARD:
	case TRANS_MOVE_DISCARD_HOLD:
	case TRANS_MOVE_SKIP:
	default: break;
	}
}


void updateRackWidget(struct gridWidget *bw, struct transMove *tm)
{
	NOT(bw);
	NOT(tm);
	
	switch (tm->type) {
	case TRANS_MOVE_PLACE_INIT:
	case TRANS_MOVE_PLACE:
	case TRANS_MOVE_PLACE_HOLD:
	case TRANS_MOVE_DISCARD_INIT:
	case TRANS_MOVE_DISCARD:
	case TRANS_MOVE_DISCARD_HOLD:
	case TRANS_MOVE_SKIP:
	default: break;
	}
}


void updateChoiceWidget(struct gridWidget *cw, struct transMove *tm)
{

	NOT(cw);
	NOT(tm);
	
	switch (tm->type) {
	case TRANS_MOVE_PLACE_INIT: {
		cw->button[0][CHOICE_RECALL]	= false;
		cw->button[0][CHOICE_MODE]	= true;
		cw->button[0][CHOICE_PLAY]	= false;
		break;
	}
	case TRANS_MOVE_PLACE: {
		cw->button[0][CHOICE_RECALL]	= true;
		cw->button[0][CHOICE_MODE]	= false;
		cw->button[0][CHOICE_PLAY]	= true;
		break;
	}
	case TRANS_MOVE_PLACE_HOLD: {
		cw->button[0][CHOICE_RECALL]	= true;
		cw->button[0][CHOICE_MODE]	= false;
		cw->button[0][CHOICE_PLAY]	= false; 
		break;
	}
	case TRANS_MOVE_DISCARD_INIT: {
		cw->button[0][CHOICE_RECALL]	= false;
		cw->button[0][CHOICE_MODE]	= true;
		cw->button[0][CHOICE_PLAY]	= false;
		break;
	}
	case TRANS_MOVE_DISCARD: {
		cw->button[0][CHOICE_RECALL]	= true;
		cw->button[0][CHOICE_MODE]	= false;
		cw->button[0][CHOICE_PLAY]	= true;
		break;
	}
	case TRANS_MOVE_DISCARD_HOLD: {
		cw->button[0][CHOICE_RECALL]	= true;
		cw->button[0][CHOICE_MODE]	= false;
		cw->button[0][CHOICE_PLAY]	= false;
		break;
	}
	case TRANS_MOVE_SKIP: {
		cw->button[0][CHOICE_RECALL]	= false;
		cw->button[0][CHOICE_MODE]	= true;
		cw->button[0][CHOICE_PLAY]	= true;
		break;
	}
	default: break;
	}


}


void boardWidgetDraw(struct io *io, struct gridWidget *bw, struct board *b, struct coor pos, struct coor dim)
{
	struct tile *t;
	struct SDL_Surface *ts;
	int y, x;
	NOT(io);
	NOT(bw);
	NOT(b);

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			t = &b->tile[y][x];
			if (t->type != TILE_NONE) {
				ts = io->tile[t->type][t->letter];
				surfaceDraw(io->screen, ts, x * dim.x + pos.x, y * dim.y + pos.y);
			}
		}
	}
}


void rackWidgetDraw(struct io *io, struct transMove *tm, struct gridWidget *rw, struct coor pos, struct coor dim)
{
	int i;
	struct tile *t;

	NOT(io);
	NOT(tm);

	for (i = 0; i < RACK_SIZE; i++) {
		t = &tm->tile[i];
		if (t->type != TILE_NONE) {
			surfaceDraw(io->screen, io->tile[t->type][t->letter], i * dim.x + 164, 220);
		}
	}
}


void choiceWidgetDraw(struct io *io, struct transMove *tm, struct gridWidget *cw, struct coor pos, struct coor dim)
{
	bool mode, recall, play;
	ModeType type;

	NOT(io);
	NOT(tm);
	NOT(cw);

	type	= MODE_INVALID;
	recall	= cw->button[0][CHOICE_RECALL];
	mode	= cw->button[0][CHOICE_MODE]; 
	play	= cw->button[0][CHOICE_PLAY];

	switch (tm->type) {
	case TRANS_MOVE_PLACE_INIT:
	case TRANS_MOVE_PLACE:
	case TRANS_MOVE_PLACE_HOLD: type = MODE_PLACE; break;
	case TRANS_MOVE_DISCARD_INIT:
	case TRANS_MOVE_DISCARD:
	case TRANS_MOVE_DISCARD_HOLD: type = MODE_DISCARD; break;
	case TRANS_MOVE_SKIP: type = MODE_SKIP; break;
	default: break;
	}

	if (mode) {
		surfaceDraw(io->screen, io->mode, 119, 217);
		switch (type) {
		case MODE_PLACE:	surfaceDraw(io->screen, io->place, 119, 220); break;
		case MODE_DISCARD:	surfaceDraw(io->screen, io->discard, 119, 220); break;
		case MODE_SKIP:		surfaceDraw(io->screen, io->skip, 119, 220); break;
		default: break;
		}
	} else {
		surfaceDraw(io->screen, io->modeDisable, 119, 217);
		switch (type) {
		case MODE_PLACE:surfaceDraw(io->screen, io->placeDisable, 119, 220); break;
		case MODE_DISCARD:surfaceDraw(io->screen, io->discardDisable, 119, 220); break;
		case MODE_SKIP:	surfaceDraw(io->screen, io->skip, 119, 220); break;
		default: break;
		}
	}
	surfaceDraw(io->screen, recall ? io->recall : io->recallDisable, 105, 220);
	surfaceDraw(io->screen, play ? io->play : io->playDisable, 133, 220);
}


void gridWidgetDraw(SDL_Surface *s, struct gridWidget *gw, struct coor pos, struct coor dim)
{
	int y, x;
	SDL_Rect clip;
	Uint32 red, green, color;
	
	NOT(gw);

	clip.w = dim.x - 1;
	clip.h = dim.y - 1;
	red   = SDL_MapRGB(s->format, 255, 0, 0);
	green = SDL_MapRGB(s->format, 0, 255, 0);
	for (y = 0; y < gw->height; y++) {
		for (x = 0; x < gw->width; x++) {
			color  = gw->button[y][x] ? green : red;
			clip.x = pos.x + dim.x * x;
			clip.y = pos.y + dim.y * y;
			SDL_FillRect(s, &clip, color);
		}
	}
}

