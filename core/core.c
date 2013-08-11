#include "common.h"
#include "print.h"

int defaultLetterScore(Letter l)
{
	switch (l) {
	case LETTER_A: return 1;
	case LETTER_B: return 3;
	case LETTER_C: return 3;
	case LETTER_D: return 2;
	case LETTER_E: return 1;
	case LETTER_F: return 4;
	case LETTER_G: return 2;
	case LETTER_H: return 4;
	case LETTER_I: return 1;
	case LETTER_J: return 8;
	case LETTER_K: return 5;
	case LETTER_L: return 1;
	case LETTER_M: return 3;
	case LETTER_N: return 1;
	case LETTER_O: return 1;
	case LETTER_P: return 3;
	case LETTER_Q: return 10;
	case LETTER_R: return 1;
	case LETTER_S: return 1;
	case LETTER_T: return 1;
	case LETTER_U: return 1;
	case LETTER_V: return 4;
	case LETTER_W: return 4;
	case LETTER_X: return 8;
	case LETTER_Y: return 4;
	case LETTER_Z: return 10;
	default: break;
	}
	return 0;
}

int tileScore(Tile *t)
{
	NOT(t);

	if (t->type == TILE_LETTER) {
		return defaultLetterScore(t->letter);
	}
	return 0;
}

bool canUseDblLet(Board *b, Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == SQ_DBL_LET;
}

bool canUseTrpLet(Board *b, Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == SQ_TRP_LET;
}

bool canUseDblWrd(Board *b, Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == SQ_DBL_WRD;
}
	

bool canUseTrpWrd(Board *b, Dir *d, int p, int x, int y)
{
	NOT(b);
	NOT(d);
	VALID_BOARD_SIZE(p);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	return d->pos[p] && b->sq[y][x] == SQ_TRP_WRD;
}

int dirScore(Board *b, Dir *d)
{
	int dw, tw, x, y, s, i, t;

	NOT(b);
	NOT(d);

	dw = 0;
	tw = 0;
	x = d->x;
	y = d->y;
	s = 0;
	switch (d->type) {
	case DIR_RIGHT: {
		for (i = d->x; i < d->len + d->x; i++) {
			t = tileScore(&b->tile[y][i]);
			t *= canUseDblLet(b, d, i, i, y) ? 2 : 1;
			t *= canUseTrpLet(b, d, i, i, y) ? 3 : 1;
			dw += canUseDblWrd(b, d, i, i, y);
			tw += canUseTrpWrd(b, d, i, i, y);
			s += t;
		}
		break;
	}
	case DIR_DOWN: {
		for (i = d->y; i < d->len + d->y; i++) {
			t = tileScore(&b->tile[i][x]);
			t *= canUseDblLet(b, d, i, x, i) ? 2 : 1;
			t *= canUseTrpLet(b, d, i, x, i) ? 3 : 1;
			dw += canUseDblWrd(b, d, i, x, i);
			tw += canUseTrpWrd(b, d, i, x, i);
			s += t;
		}
		break;
	}
	case DIR_INVALID: /* fall through */
	default: return 0;
	}

	if (dw > 0) {
		s *= dw * 2;
	}
	if (tw > 0) {
		s *= tw * 3;
	}
	return s;
}

int metaPathScore(Dir *d, Dir *adj, int n, Board *b)
{
	int i, s;

	NOT(d);
	NOT(adj);
	NOT(b);

	s = 0;
	if (d->type != DIR_INVALID) {
		s = dirScore(b, d);
		for (i = 0; i < n; i++) {
			if (adj[i].type != DIR_INVALID) {
				s += dirScore(b, &adj[i]);
			}
		}
	}
	return s;
}

int pathScore(Path *p)
{
	int s;
	Board *b;

	NOT(p);

	s = 0;
	b = &p->board;

	switch (p->type) {
	case PATH_DOT: {
		if (p->data.dot.right.type == DIR_RIGHT) {
			s = dirScore(b, &p->data.dot.right);
		}
		if (p->data.dot.down.type == DIR_DOWN) {
			s += dirScore(b, &p->data.dot.down);
		}
		break;
	}
	case PATH_HORZ: {
		s = metaPathScore(&p->data.horz.right, p->data.horz.down, BOARD_X, b);
		break;
	}
	case PATH_VERT: {
		s = metaPathScore(&p->data.vert.down, p->data.vert.right, BOARD_Y, b);
		break;
	}
	case PATH_INVALID: /* fall through */
	default: return 0;
	}
	return s;
}

bool bagFull(Bag *b)
{
	NOT(b);

	return (b->head + 1) % BAG_SIZE == b->tail;
}

bool bagEmpty(Bag *b)
{
	NOT(b);

	return b->head == b->tail;
}

int bagSize(Bag *b)
{
	NOT(b);

	if (b->head > b->tail) {
		return b->head - b->tail;
	}
	return b->tail - b->head;
}

Tile bagPeek(Bag *b)
{
	Tile next;

	NOT(b);

	next.type = TILE_NONE;
	if (b->head != b->tail) {
		next = b->tile[b->head];
	}
	return next;
}

void bagDrop(Bag *b)
{
	NOT(b);

	b->head ++;
	b->head %= BAG_SIZE;
}

void bagAdd(Bag *b, Tile t)
{
	NOT(b);

	b->tile[b->tail] = t;
	b->tail ++;
	b->tail %= BAG_SIZE;
}

bool adjustOutOfRange(Adjust *a)
{
	int i;

	NOT(a);
	assert(a->type == ADJUST_RACK);

	for (i = 0; i < RACK_SIZE; i++) {
		if (a->data.tile[i].idx < 0) {
			return true;
		}
		if (a->data.tile[i].idx >= RACK_SIZE) {
			return true;
		}
	}
	return false;
}

void adjustSwap(Adjust *a, int i, int j)
{
	TileAdjust tmp;

	NOT(a);
	assert(a->type == ADJUST_RACK);
	VALID_RACK_SIZE(i);
	VALID_RACK_SIZE(j);

	tmp = a->data.tile[i];
	a->data.tile[i] = a->data.tile[j];
	a->data.tile[j] = tmp;
}

void mkAdjust(Adjust *a, Player *p)
{
	int i;

	NOT(a);
	NOT(p);

	a->type = ADJUST_RACK;
	for (i = 0; i < RACK_SIZE; i++) {
		a->data.tile[i].type = p->tile[i].type;
		a->data.tile[i].idx = i;
	}
}

bool adjustDuplicateIndex(Adjust *a)
{
	int i, j, idx;

	NOT(a);

	assert(a->type == ADJUST_RACK);

	for (i = 0; i < RACK_SIZE; i++) {
		idx = a->data.tile[i].idx;
		assert(idx >= 0);
		assert(idx < RACK_SIZE);
		for (j = i + 1; j < RACK_SIZE; j++) {
			if (idx == a->data.tile[j].idx) {
				return true;
			}
		}
	}
	return false;
}

AdjustErrType fdAdjustErr(Adjust *a, Player *p)
{
	NOT(a);
	NOT(p);
	
	assert(a->type == ADJUST_RACK);
	
	if (adjustOutOfRange(a)) {
		return ADJUST_ERR_RACK_OUT_OF_RANGE;
	}
	if (adjustDuplicateIndex(a)) {
		return ADJUST_ERR_RACK_DUPLICATE_INDEX;
	}
	return ADJUST_ERR_NONE;
}

void applyAdjust(Player *p, Adjust *a)
{
	int i, idx;
	Tile tile[RACK_SIZE];

	NOT(p);
	NOT(a);
	assert(a->type == ADJUST_RACK);

	for (i = 0; i < RACK_SIZE; i++) {
		idx = a->data.tile[i].idx;
		tile[i] = p->tile[idx];
	}
	memCpy(p->tile, tile, sizeof(tile));
}

CmpType cmpWord(Word *w0, Word *w1) 
{
	/*
	w0 > w1 -> CMP_GREATER
	w0 < w1 -> CMP_LESS
	w0 == w1 -> CMP_EQUAL
	*/
	int i;

	NOT(w0);
	NOT(w1);

	for (i = 0; ; i++) {
		if (w0->len  > w1->len && i == w1->len) {
			return CMP_GREATER;
		}
		if (w0->len  < w1->len && i == w0->len) {
			return  CMP_LESS;
		}
		if (w0->len == w1->len && i == w1->len) {
			return CMP_EQUAL;
		}
		if (w0->letter[i] == w1->letter[i]) {
			continue;
		}
		if (w0->letter[i]  > w1->letter[i]) {
			return CMP_GREATER;
		}
		if (w0->letter[i]  < w1->letter[i]) {
			return CMP_LESS;
		}
	}
	return CMP_EQUAL;
}

bool wordValid(Word *w, Dict *d)
{
	long min, max, mid;
	
	NOT(w);
	NOT(d);
	
	min = 0;
	max = d->num;
	mid = d->num / 2;

	while (min <= max) {
		switch (cmpWord(w, &d->words[mid])) {
		case CMP_EQUAL: return true;
		case CMP_GREATER: min = mid + 1; break;
		case CMP_LESS: max = mid - 1; break;
		default: return false; /* Should never arrive here via cmpWord */
		}
		mid = (min + max) / 2;
	}
	return false;
}

bool dirValid(Dir *dir, Board *b, Dict *dict, Word *w)
{
	int x, y, i;

	NOT(dir);
	NOT(b);
	NOT(dict);

	x = dir->x;
	y = dir->y;
	w->len = dir->len;
	switch (dir->type) {
	case DIR_RIGHT: {
		for (i = 0; i < w->len; i++) {
			if (b->tile[y][x + i].type != TILE_NONE) {
				w->letter[i] = b->tile[y][x + i].letter;
			} else {
				return false;
			}
		}
		break;
	}
	case DIR_DOWN: {
		for (i = 0; i < w->len; i++) {
			if (b->tile[y + i][x].type != TILE_NONE) {
				w->letter[i] = b->tile[y + i][x].letter;
			} else {
				return false;
			}
		}
		break;
	}
	case DIR_INVALID: /* fall through */
	default: return false;
	}
	return true;
}

bool pathValid(Path *p, Dict *d, bool (*rule)(Word *, PathType, DirType))
{
	int i;
	Word w0, w1;
	bool result;

	NOT(p);
	NOT(d);

	result = true;
	switch (p->type) {
	case PATH_DOT: {
		bool a0, a1, b0, b1;
		a0 = dirValid(&p->data.dot.right, &p->board, d, &w0);
		b0 = dirValid(&p->data.dot.down, &p->board, d, &w1);

		a0 = a0 ? wordValid(&w0, d) : a0;
		b0 = b0 ? wordValid(&w1, d) : b0;

		a0 = a0 && rule ? rule(&w0, p->type, DIR_RIGHT) : a0;
		b0 = b0 && rule ? rule(&w1, p->type, DIR_DOWN) : b0;

		a1 = p->data.dot.right.len > 1;
		b1 = p->data.dot.down.len > 1;

		if (!((a0 && !(b0 || b1)) || (b0 && !(a0 || a1)) || (a0 && b0))) {
			result = false;	
		}
		break;
	}
	case PATH_HORZ: {
		if (!dirValid(&p->data.horz.right, &p->board, d, &w0)) {
			result = false;
		} else if (!wordValid(&w0, d)) {
			result = false;
		} else if (rule && !rule(&w0, p->type, DIR_RIGHT)) {
			result = false;
		}
		for (i = 0; i < BOARD_X; i++) {
			if (p->data.horz.down[i].type == DIR_DOWN) {
				if (!dirValid(&p->data.horz.down[i], &p->board, d, &w1)) {
					result = false;
				} else if (w1.len > 1 && !wordValid(&w1, d)) {
					result = false;
				} else if (w1.len > 1 && rule && !rule(&w1, p->type, DIR_DOWN)) {
					result = false;
				}
			}
		}
		break;
	}
	case PATH_VERT: {
		if (!dirValid(&p->data.vert.down, &p->board, d, &w0)) {
			result = false;
		} else if (!wordValid(&w0, d)) {
			result = false;
		} else if (rule && !rule(&w0, p->type, DIR_DOWN)) {
			result = false;
		}
		for (i = 0; i < BOARD_Y; i++) {
			if (p->data.vert.right[i].type == DIR_RIGHT) {
				if(!dirValid(&p->data.vert.right[i], &p->board, d, &w1)) {
					result = false;
				} else if (w1.len > 1 && !wordValid(&w1, d)) {
					result = false;
				} else if (w1.len > 1 && rule && !rule(&w1, p->type, DIR_RIGHT)) {
					result = false;
				}
			}
		}
		break;
	}
	case PATH_INVALID: /* fall through */
	default: result = false; break; 
	}
	return result; 
}

/*
PathErrType pathValidWithErr(Path *p, Dict *d)
{
	int i;
	Word w0, w1;

	NOT(p);
	NOT(d);

	switch (p->type) {
	case PATH_DOT: {
		bool a0, a1, b0, b1;
		a0 = dirValid(&p->data.dot.right, &p->board, d, &w0);
		b0 = dirValid(&p->data.dot.down, &p->board, d, &w1);
		a0 = a0 ? wordValid(&w0, d) : a0;
		b0 = b0 ? wordValid(&w1, d) : b0;
		a1 = p->data.dot.right.len > 1;
		b1 = p->data.dot.down.len > 1;

		if (!((a0 && !(b0 || b1)) || (b0 && !(a0 || a1)) || (a0 && b0))) {
			return false;
		}
		break;
	}
	case PATH_HORZ: {
		if (!dirValid(&p->data.horz.right, &p->board, d, &w0)) {
			return PATH_ERR_NON_CONT;
		}
		if (!wordValid(&w0, d)) {
			return PATH_ERR_INVALID_WORD;
		}
		for (i = 0; i < BOARD_X; i++) {
			if (p->data.horz.down[i].type == DIR_DOWN) {
				if (!dirValid(&p->data.horz.down[i], &p->board, d, &w1)) {
					return PATH_ERR_NON_CONT;
				}
				if (!wordValid(&w1, d)) {
					return PATH_ERR_INVALID_WORD;
				}
			}
		}
		break;
	}
	case PATH_VERT: {
		if (!dirValid(&p->data.vert.down, &p->board, d, &w0)) {
			return PATH_ERR_NON_CONT; 
		}
		if (!wordValid(&w0, d)) {
			return PATH_ERR_INVALID_WORD;
		}
		for (i = 0; i < BOARD_Y; i++) {
			if (p->data.vert.right[i].type == DIR_RIGHT) {
				if(!dirValid(&p->data.vert.right[i], &p->board, d, &w1)) {
					return PATH_ERR_NON_CONT;
				}
				if (!wordValid(&w1, d)) {
					return PATH_ERR_INVALID_WORD;
				}
			}
		}
		break;
	}
	case PATH_INVALID:
	default: return PATH_ERR_INVALID_PATH;
	}
	return PATH_ERR_NONE;
}
*/

bool tilesAdjacent(Board *b, MovePlace *mp, Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		if (p->tile[r].type != TILE_NONE) {
			if (x > 0 && b->tile[y][x - 1].type != TILE_NONE) {
				return true;
			}
			if (y > 0 && b->tile[y - 1][x].type != TILE_NONE) {
				return true;
			}
			if (x < BOARD_X - 1 &&
					b->tile[y][x + 1].type != TILE_NONE) {
				return true;
			}
			if (y < BOARD_Y - 1 &&
					b->tile[y + 1][x].type != TILE_NONE) {
				return true;
			}
		}
			
	}
	return false;
}

bool onFreeSquares(Board *b, MovePlace *mp, Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		if (p->tile[r].type != TILE_NONE && b->sq[y][x] == SQ_FREE) {
			return true;
		}
	}
	return false;
}

bool placeInRange(MovePlace *mp)
{
	int i;
	
	NOT(mp);
	
	for (i = 0; i < mp->num; i++) {
		if (mp->rackIdx[i] < 0 || mp->rackIdx[i] >= RACK_SIZE) {
			return false;
		}
		if (mp->coor[i].x < 0 || mp->coor[i].x >= BOARD_X) {
			return false;
		}
		if (mp->coor[i].y < 0 || mp->coor[i].y >= BOARD_Y) {
			return false;
		}
	}
	return true;
}

bool placeOverlap(MovePlace *mp)
{
	int i, j;

	NOT(mp);
	
	for (i = 0; i < mp->num; i++) {
		for (j = i + 1; j < mp->num; j++) {
			if (mp->rackIdx[i] == mp->rackIdx[j]) {
				return false;
			}
			if (mp->coor[i].x == mp->coor[j].x &&
			    mp->coor[i].y == mp->coor[j].y) {
				return true;
			}
		}
	}
	return false;
}

bool placeOverlapBoard(MovePlace *mp, Board *b)
{
	int i, x, y;

	NOT(mp);
	NOT(b);
	
	for (i = 0; i < mp->num; i++) {
		x = mp->coor[i].x;
		y = mp->coor[i].y;
		if (b->tile[y][x].type != TILE_NONE) {
			return true;
		}
	}
	return false;
}

bool placeRackExist(MovePlace *mp, Player *p)
{
	int i, r;

	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		if (p->tile[r].type == TILE_NONE) {
			return false;
		}
	}
	return true;
}

void cpyRackBoard(Board *b, MovePlace *mp, Player *p)
{
	int x, y, i, r;

	NOT(b);
	NOT(mp);
	NOT(p);

	for (i = 0; i < mp->num; i++) {
		r = mp->rackIdx[i];
		y = mp->coor[i].y;
		x = mp->coor[i].x;
		memCpy(&b->tile[y][x], &p->tile[r], sizeof(Tile));
	}
}

bool isHorz(Action *a, Move *m)
{
	int i, min, max, y;
	Coor *coor;
	Board *b;

	NOT(a);
	NOT(m);

	b = &a->data.place.path.board;
	y = m->data.place.coor[0].y;
	min = max = m->data.place.coor[0].x;
	if (m->data.place.num < 2) {
		return false;
	}
	for (i = 1; i < m->data.place.num; i++) {
		coor = &m->data.place.coor[i];
		if (y != coor->y) {
			return false;
		}
		if (min < coor->x) {
			min = coor->x;
		}
		if (max > coor->x) {
			max = coor->x;
		}
	}
	for (i = min; i <= max; i++) {
		if (b->tile[y][i].type == TILE_NONE) {
			return false;
		}
	}
	return true;
}

bool isVert(Action *a, Move *m)
{
	int i, min, max, x;
	Board *board;
	Coor *coor;
	
	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	board = &a->data.place.path.board;
	min = max = m->data.place.coor[0].y;

	if (m->data.place.num < 2) {
		return false;
	}
	for (i = 1; i < m->data.place.num; i++) {
		coor = &m->data.place.coor[i];
		if (x != coor->x) {
			return false;
		}
		if (min > coor->y) {
			min = coor->y;
		}
		if (max < coor->y) {
			max = coor->y;
		}
	}
	for (i = min; i <= max; i++) {
		if (board->tile[i][x].type == TILE_NONE) {
			return false;
		}
	}
	return true;
}

void mkRight(Dir *d, int x, int y, Board *b)
{
	int i;

	NOT(d);
	NOT(b);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	d->type = DIR_RIGHT;
	d->x = x;
	d->y = y;
	memSet(d->pos, false, sizeof(bool) * BOARD_SIZE);
	d->pos[x] = true;

	for (i = x; i >= 0 && b->tile[y][i].type != TILE_NONE; i--) {
		d->x = i;
	}

	for (i = x; i < BOARD_X && b->tile[y][i].type != TILE_NONE; i++) {
		d->len = i;
	}
	d->len -= d->x - 1;

	/* a word cannot be 1 letter long */
	if (d->len == 1) {
		 d->type = DIR_INVALID;
	}
}

void mkDown(Dir *d, int x, int y, Board *b)
{
	int i;

	NOT(d);
	NOT(b);
	VALID_BOARD_X(x);
	VALID_BOARD_Y(y);

	d->type = DIR_DOWN;
	d->x = x;
	d->y = y;
	memSet(d->pos, false, sizeof(bool) * BOARD_SIZE);
	d->pos[y] = true;

	for (i = y; i >= 0 && b->tile[i][x].type != TILE_NONE; i--) {
		d->y = i;
	}

	for (i = y; i < BOARD_Y && b->tile[i][x].type != TILE_NONE; i++) {
		d->len = i;
	}
	d->len -= d->y - 1;

	/* a word cannot be 1 letter long */
	if (d->len == 1) {	
		d->type = DIR_INVALID;
	}
}

void mkDot(Action *a, Move *m)
{
	int x, y;
	Path *p;
	Board *b;
	Dir *d;

	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	p = &a->data.place.path; 
	b = &a->data.place.path.board;
	d = NULL;

	p->type = PATH_DOT;

	d = &p->data.dot.right;
	mkRight(d, x, y, b);

	d = &p->data.dot.down;
	mkDown(d, x, y, b);
}

void mkHorz(Action *a, Move *m)
{
	int i, x, y;
	Path *p;
	Board *b;
	Dir *d;

	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	p = &a->data.place.path; 
	b = &a->data.place.path.board;

	p->type = PATH_HORZ;

	d = &p->data.horz.right;
	mkRight(d, x, y, b);

	for (i = 0; i < m->data.place.num; i++) {
		x = m->data.place.coor[i].x;
		d->pos[x] = true;
	}
	for (i = 0; i < BOARD_X; i++) {
		p->data.horz.down[i].type = DIR_INVALID;
	}
	for (i = 0; i < m->data.place.num; i++) {
		d = &p->data.horz.down[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mkDown(d, x, y, b);
	}
}

void mkVert(Action *a, Move *m)
{
	int i, x, y;
	Path *path;
	Board *b;
	Dir *d;

	NOT(a);
	NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	b = &a->data.place.path.board;
	d = NULL;

	path = &a->data.place.path; 
	d = &path->data.vert.down;

	path->type = PATH_VERT;
	mkDown(d, x, y, b);

	for (i = 0; i < m->data.place.num; i++) {
		y = m->data.place.coor[i].y;
		d->pos[y] = true;
	}
	for (i = 0; i < BOARD_Y; i++) {
		path->data.vert.right[i].type = DIR_INVALID;
	}
	for (i = 0; i < m->data.place.num; i++) {
		d = &path->data.vert.right[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mkRight(d, x, y, b);
	}
}

ActionErrType fdPlaceErr(MovePlace *mp,Player *p, Board *b)
{
	NOT(mp);
	NOT(p);
	NOT(b);
	
	if (!placeInRange(mp)) {
		return ACTION_ERR_PLACE_OUT_OF_RANGE;
	}
	if (placeOverlap(mp)) {
		return ACTION_ERR_PLACE_SELF_OVERLAP;
	}
	if (placeOverlapBoard(mp, b)) {
		return ACTION_ERR_PLACE_BOARD_OVERLAP;
	}
	if (!placeRackExist(mp, p)) {
		return ACTION_ERR_PLACE_INVALID_RACK_ID;
	}
	if (!tilesAdjacent(b, mp, p) && !onFreeSquares(b, mp, p)) {
		return ACTION_ERR_PLACE_INVALID_SQ;
	}
	return ACTION_ERR_NONE;
}

bool ruleZ4Char(Word *w, PathType pt, DirType dt)
{
	NOT(w);
	printWord(w);
	putchar('\n');
	return w->len == 4 && w->letter[0] == LETTER_Z;
}

void mkPlace(Action *a, Game *g, Move *m)
{
	int num, i;
	Path *path;
	Player *player;
	ActionErrType err;

	NOT(a);
	NOT(g);
	NOT(m);

	num = m->data.place.num;
	path = &a->data.place.path;
	player = &g->player[m->playerIdx];
	a->type = ACTION_PLACE;

	a->data.place.num = m->data.place.num;
	for (i = 0; i < num; i++) {
		a->data.place.rackIdx[i] = m->data.place.rackIdx[i];
	}
	err = fdPlaceErr(&m->data.place, player, &g->board);
	if (err != ACTION_ERR_NONE) {
		a->type = ACTION_INVALID;
		a->data.err = err;
		return;
	}

	memCpy(&path->board, &g->board, sizeof(Board));
	cpyRackBoard(&path->board, &m->data.place, player);

	assert(num >= 0);
	switch (num) {
	case 0: {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_PLACE_NO_RACK;
		return;
	}
	case 1: {
		mkDot(a, m);
		break;
	}
	default: {
		assert(num > 1);
		if (isHorz(a, m)) {
			mkHorz(a, m);
		} else { 
			if (isVert(a, m)) {
				mkVert(a, m);
			} else {
				a->type = ACTION_INVALID;
				a->data.err = ACTION_ERR_PLACE_NO_DIR;
				return;
			}
		}
		break;
	}
	}

	if (!pathValid(path, &g->dict, NULL)) {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_PLACE_INVALID_PATH;
		return;
	}
	a->data.place.score = pathScore(path);
}

void mkDiscard(Action *a, Game *g, Move *m)
{

	NOT(a);
	NOT(g);
	NOT(m);

	a->type = ACTION_DISCARD;
	a->data.discard.num = m->data.discard.num;
	assert(sizeof(a->data.discard) == sizeof(m->data.discard));
	memCpy(&a->data.discard, &m->data.discard, sizeof(m->data.discard));
}

void mkSkip(Action *a, Game *g)
{
	NOT(a);
	NOT(g);
	
	if (g->rule.skip && !g->rule.skip(g)) {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_SKIP_RULE;
		return;
	}
	a->type = ACTION_SKIP;
}

void mkQuit(Action *a, Game *g)
{
	NOT(a);
	NOT(g);
	
	if (g->rule.quit && !g->rule.quit(g)) {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_QUIT_RULE;
		return;
	}
	a->type = ACTION_QUIT;
}

void mkAction(Action *a, Game *g, Move *m)
{
	NOT(a);
	NOT(g);
	NOT(m);

	a->playerIdx = m->playerIdx;
	switch (m->type) {
	case MOVE_PLACE: mkPlace(a, g, m); break;
	case MOVE_DISCARD: mkDiscard(a, g, m); break;
	case MOVE_SKIP: mkSkip(a, g); break;
	case MOVE_QUIT: mkQuit(a, g); break;
	case MOVE_INVALID: /* fall through */
	default: a->type = ACTION_INVALID; break;
	}
}

void rackShift(Player *p)
{
	int i, j;

	NOT(p);

	j = 0;
	i = 0;
	while (i < RACK_SIZE) {
		while (p->tile[i].type == TILE_NONE && i < RACK_SIZE) {
			i++;
		}
		if (i == RACK_SIZE) {
			break;
		}
		memCpy(&p->tile[j], &p->tile[i], sizeof(Tile));
		i++;
		j++;
	}
	while (j < RACK_SIZE) {
		p->tile[j].type = TILE_NONE;
		j++;
	}
}

void rackRefill(Player *p, Bag *b)
{
	int i;
	
	NOT(p);
	NOT(b);
	
	for (i = 0; i < RACK_SIZE && !bagEmpty(b); i++) {
		if (p->tile[i].type == TILE_NONE) {
			p->tile[i] = bagPeek(b);
			bagDrop(b);
		}
	}
}

bool applyAction(Game *g, Action *a)
{
	int id, i, r;

	NOT(g);
	NOT(a);

	id = a->playerIdx;
	if (id != g->turn) {
		return false;
	}
	switch (a->type) {
	case ACTION_PLACE: {
		memCpy(&g->board, &a->data.place.path.board,
				sizeof(Board));
		g->player[id].score += a->data.place.score;
		for (i = 0; i < a->data.place.num; i++) {
			r = a->data.place.rackIdx[i];
			g->player[id].tile[r].type = TILE_NONE;
		}
		rackRefill(&g->player[id], &g->bag);
		rackShift(&g->player[id]);
		break;
	}
	case ACTION_DISCARD: {
		for (i = 0; i < a->data.discard.num; i++) {
			r = a->data.discard.rackIdx[i];
			g->player[id].tile[r].type = TILE_NONE;
		}
		rackRefill(&g->player[id], &g->bag);
		rackShift(&g->player[id]);
		g->player[id].score -= a->data.discard.num;
		if (g->player[id].score < 0) {
			g->player[id].score = 0;
		}
		break;
	}
	case ACTION_SKIP: {
		break;
	}
	case ACTION_QUIT: {
		g->player[id].active = false;
		break;
	}
	case ACTION_INVALID: /* fall through */
	default: return false;
	}
	return true;
}

void rmRackTile(Player *p, int *rackIdx, int n)
{
	int i;

	NOT(p);
	NOT(rackIdx);

	for (i = 0; i < n; i++) {
		p->tile[rackIdx[i]].type = TILE_NONE;
	}
}

void nextTurn(Game *g)
{
	NOT(g);

	do {
		g->turn ++;
		g->turn %= g->playerNum;
	} while (!g->player[g->turn].active);
}

void moveClr(Move *m)
{
	NOT(m);

	memSet(m, 0, sizeof(Move));
	m->type = MOVE_INVALID;
}

void actionClr(Action *a)
{
	NOT(a);

	memSet(a, 0, sizeof(Action));
	a->type = ACTION_INVALID;
}

bool endGame(Game *g)
{
	int i, j, k;

	NOT(g);

	/* 2 active players min. */	
	j = 0;
	for (i = 0; i < g->playerNum; i++) {
		if (g->player[i].active) {
			j++;
		}
	}
	if (j < 2) {
			return true;
	}
	/* 1 player wth tiles on the rack */
	j = 0;
	for (i = 0; i < g->playerNum; i++) {
		if (g->player[i].active) {
			for (k = 0; k < RACK_SIZE; k++) {
				if (g->player[i].tile[k].type != TILE_NONE) {
					j++;
				}
			}
		}
	}
	if (j == 0) {
		return true;
	}
	return false;
}

int fdWinner(Game *g)
{
	int max, idx, i, j;

	NOT(g);

	/* if exist winner then winnerIdx else -1 */

	idx = -1;
	j = 0;

	for (i = 0; i < g->playerNum; i++) {
		if (g->player[i].active) {
			idx = i;
			j++;
		}
	} 
	if (j <= 1) {
		return idx;
	}
	idx = -1;
	max = 0;
	for (i = 0; i < g->playerNum; i++) {
		if (g->player[i].active) {
			if (max < g->player[i].score) {
				idx = i;
				max = g->player[i].score;
			}
		}
	} 
	return idx;
}

bool validRackIdx(int id)
{
	return id >= 0 && id < RACK_SIZE;
}

bool validBoardIdx(Coor c)
{
	return c.x >= 0 && c.y >= 0 && c.x < BOARD_X && c.y < BOARD_Y;
}

int rackCount(Player *p)
{
	int i, count;

	NOT(p);
	
	count = 0;
	for (i = 0; i < RACK_SIZE; i++) {
		assert(p->tile[i].type == TILE_NONE || p->tile[i].type == TILE_WILD || p->tile[i].type == TILE_LETTER);
		if (p->tile[i].type != TILE_NONE) {
			count++;
		}
	}
	return count;
}

int adjustTileCount(Adjust *a)
{
	int i, count;

	NOT(a);
	
	count = 0;
	for (i = 0; i < RACK_SIZE; i++) {
		assert(a->data.tile[i].type == TILE_NONE || a->data.tile[i].type == TILE_WILD || a->data.tile[i].type == TILE_LETTER);
		if (a->data.tile[i].type != TILE_NONE) {
			count++;
		}
	}
	return count;
}

bool boardEmpty(Board *b)
{
	int y, x;

	NOT(b);

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			if (b->tile[y][x].type != TILE_NONE) {
				return false;
			}
		}
	}

	return true;
}

