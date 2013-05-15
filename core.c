#include "core.h"


/* tile_functions */


int tile_score(struct tile *t)
{
	NOT(t);
	if (t->type == TILE_LETTER) {
		switch (t->letter) {
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
	}
	return 0;
}


/* dir_functions */


bool can_use_dbl_let(struct board *b, struct dir *s, int p, int x, int y)
{
	NOT(b), NOT(s), VALID_BOARD_SIZE(p), VALID_BOARD_X(x), VALID_BOARD_Y(y);
	return s->pos[p] && b->sq[y][x] == SQ_DBL_LET;
}


bool can_use_trp_let(struct board *b, struct dir *s, int p, int x, int y)
{
	NOT(b), NOT(s), VALID_BOARD_SIZE(p), VALID_BOARD_X(x), VALID_BOARD_Y(y);
	return s->pos[p] && b->sq[y][x] == SQ_TRP_LET;
}


bool can_use_dbl_wrd(struct board *b, struct dir *s, int p, int x, int y)
{
	NOT(b), NOT(s), VALID_BOARD_SIZE(p), VALID_BOARD_X(x), VALID_BOARD_Y(y);
	return s->pos[p] && b->sq[y][x] == SQ_DBL_WRD;
}
	

bool can_use_trp_wrd(struct board *b, struct dir *s, int p, int x, int y)
{
	NOT(b), NOT(s), VALID_BOARD_SIZE(p), VALID_BOARD_X(x), VALID_BOARD_Y(y);
	return s->pos[p] && b->sq[y][x] == SQ_TRP_WRD;
}


int score_dir(struct board *b, struct dir *d)
{
	int dw, tw, x, y, score;
	NOT(b), NOT(d);
	dw = 0;
	tw = 0;
	x = d->x;
	y = d->y;
	score = 0;
	switch (d->type) {
	case DIR_RIGHT: {
		int p, i, t;
		for (p = 0, i = d->x; i < d->length + d->x; p++, i++) {
			t   = tile_score(&b->tile[y][i]);
			t  *= can_use_dbl_let(b, d, p, i, y) ? 2 : 1;
			t  *= can_use_trp_let(b, d, p, i, y) ? 3 : 1;
			dw += can_use_dbl_wrd(b, d, p, i, y);
			tw += can_use_trp_wrd(b, d, p, i, y);
			score += t;
		}
		break;
	}
	case DIR_DOWN: {
		int p, i, t;
		for (p = 0, i = d->y; i < d->length + d->y; p++, i++) {
			t   = tile_score(&b->tile[i][x]);
			t  *= can_use_dbl_let(b, d, p, x, i) ? 2 : 1;
			t  *= can_use_trp_let(b, d, p, x, i) ? 3 : 1;
			dw += can_use_dbl_wrd(b, d, p, x, i);
			tw += can_use_trp_wrd(b, d, p, x, i);
			score += t;
		}
		break;
	}
	default: break;
	}
	if (dw > 0)
		score *= dw * 2;
	if (tw > 0)
		score *= tw * 3;
	return score;
}


/* path_functions */


int score_meta_path(struct dir *d, struct dir *adj, int n, struct board *b)
{
	int i, s;
	NOT(d), NOT(adj), NOT(b);
	s = 0;
	if (d->type != DIR_INVALID) {
		s = score_dir(b, d);
		for (i = 0; i < n; i++)
			if (adj[i].type != DIR_INVALID)
				s += score_dir(b, adj+i);
		
	}
	return s;
}


int score_path(struct path *p)
{
	int score;
	struct board *b;
	NOT(p);
	score = 0;
	b = &p->board;
	switch (p->type) {
	case PATH_DOT:
		if (p->data.dot.right.type == DIR_RIGHT)
			score  = score_dir(b, &p->data.dot.right);
		if (p->data.dot.down.type == DIR_DOWN)
			score += score_dir(b, &p->data.dot.down);
		break;
	case PATH_HORZ:
		score = score_meta_path(&p->data.horz.right, p->data.horz.down, BOARD_X, b);
		break;
	case PATH_VERT:
		score = score_meta_path(&p->data.vert.down, p->data.vert.right, BOARD_Y, b);
		break;
	default: break;
	}
	return score;
}


/* bag_functions 
 * `struct bag' is implemented as a fixed-sized, circular queue.
 * Be careful not to add more tiles than BAG_SIZE-1.
 */


int bag_full(struct bag *b)
{
	NOT(b);
	return (b->head + 1) % BAG_SIZE == b->tail;
}


int bag_empty(struct bag *b)
{
	NOT(b);
	return b->head == b->tail;
}


int bag_size(struct bag *b)
{
	NOT(b);
	if (b->head > b->tail)
		return b->head - b->tail;
	return b->tail - b->head;
}


struct tile bag_peek(struct bag *b)
{
	struct tile next;
	NOT(b);
	next.type = TILE_NONE;
	if (b->head != b->tail)
		next = b->tile[b->head];
	return next;
}


void bag_drop(struct bag *b)
{
	NOT(b);
	b->head ++;
	b->head %= BAG_SIZE;
}


void bag_add(struct bag *b, struct tile t)
{
	NOT(b);
	b->tile[b->tail] = t;
	b->tail ++;
	b->tail %= BAG_SIZE;
}


void shake_bag(struct bag *b)
{
	/* todo: implement TOMORROW! @ procrastinators conference */
}


int cmp_word(letter_t w0[], int len0, letter_t w1[], int len1) 
{
	/* returns | w0 >  w1 = -1
	 *         | w0 <  w1 =  1
	 *         | w0 == w1 =  0
	 */
	int i;
	for (i = 0; ; i++) {
		if      (len0 >  len1 && i == len1)
			return -1;
		else if (len0 <  len1 && i == len0)
			return  1;
		else if (len0 == len1 && i == len0)
			return  0;
		if (w0[i] == w1[i])
			continue;
		else if (w0[i] > w1[i])
			return -1;
		else
			return  1;
	}
	return 0;
}


int valid_word(letter_t word[], int len, struct dictionary *dict)
{
	/* Glorified binary search */
	long min = 0;
	long max = dict->num;
	long mid = dict->num / 2;
	for (;;) {
		switch (cmp_word(word, len, dict->word[mid], dict->len[mid])) {
		case 0: return 1;
		case -1: min = mid + 1; break;
		case  1: max = mid - 1; break;
		default: return 0;
		}
		mid = (min + max) / 2;
		if (min > max)
			return 0;
	}
	/* Should never reach to this point */
	return 0;
}


int valid_dir(struct dir *dir, struct board *b, struct dictionary *dict)
{
	int x, y, len;
	letter_t word[BOARD_SIZE];
	NOT(dir), NOT(b), NOT(dict);
	x = dir->x;
	y = dir->y;
	len = dir->length;
	switch (dir->type) {
	case DIR_RIGHT: {
		int i;
		for (i = 0; i < len; i++) {
			if (b->tile[y][x+i].type != TILE_NONE)
				word[i] = b->tile[y][x+i].letter;
			else
				return 0;
		}
		break;
	}
	case DIR_DOWN: {
		int i;
		for (i = 0; i < len; i++) {
			if (b->tile[y+i][x].type != TILE_NONE)
				word[i] = b->tile[y+i][x].letter;
			else
				return 0;
		}
		break;
	}
	default: return 0;
	}
	return valid_word(word, len, dict);
}


int valid_path(struct path *p, struct dictionary *d)
{
	int i;
	switch (p->type) {
	case PATH_DOT:
		if (!valid_dir(&p->data.dot.right, &p->board, d))
			return 0;
		if (!valid_dir(&p->data.dot.down, &p->board, d))
			return 0;
		break;
	case PATH_HORZ:
		if (!valid_dir(&p->data.horz.right, &p->board, d))
			return 0;
		for (i = 0; i < BOARD_X; i++)
			if (p->data.horz.down[i].type == DIR_DOWN &&
			   (!valid_dir(&p->data.horz.down[i], &p->board, d)))
				return 0;
		break;
	case PATH_VERT:
		if (!valid_dir(&p->data.vert.down, &p->board, d)) 
			return 0;
		for (i = 0; i < BOARD_Y; i++)
			if (p->data.vert.right[i].type == DIR_RIGHT &&
			   (!valid_dir(&p->data.vert.right[i], &p->board, d))) 
				return 0;
		break;
	default: return 0;
	}
	return 1;
}


/* act_move_functions */


int cpy_rack_board(struct board *b, struct place *place, struct player *player)
{
	int x, y, i, r;
	for (i = 0; i < place->num; i++) {
		r = place->rack_id[i];
		y = place->coor[i].y;
		x = place->coor[i].x;
		if (player->tile[r].type != TILE_NONE) {
			if (b->tile[y][x].type != TILE_NONE)
				return 1;
			memcpy(&b->tile[y][x], &player->tile[r],
					sizeof(struct tile));
		}
	}
	return 0;
}


int is_horz(struct action *a, struct move *m)
{
	int i, min, max, y;
	struct coor *coor;
	struct board *board = &a->data.place.path.board;
	y = m->data.place.coor[0].y;
	min = max = m->data.place.coor[0].x;
	if (m->data.place.num < 2)
		return 0;
	for (i = 1; i < m->data.place.num; i++) {
		coor = &m->data.place.coor[i];
		if (y != coor->y)
			return 0;
		if (min < coor->x)
			min = coor->x;
		if (max > coor->x)
			max = coor->x;
	}
	for (i = min; i <= max; i++)
		if (board->tile[y][i].type == TILE_NONE)
			return 0;
	return 1;
}


int is_vert(struct action *a, struct move *m)
{
	int i, min, max, x;
	struct coor *coor;
	struct board *board = &a->data.place.path.board;
	x = m->data.place.coor[0].x;
	min = max = m->data.place.coor[0].y;
	if (m->data.place.num < 2)
		return 0;
	for (i = 1; i < m->data.place.num; i++) {
		coor = &m->data.place.coor[i];
		if (x != coor->x)
			return 0;
		if (min > coor->y)
			min = coor->y;
		if (max < coor->y)
			max = coor->y;
	}
	for (i = min; i <= max; i++)
		if (board->tile[i][x].type == TILE_NONE)
			return 0;
	return 1;
}


void mk_right(struct dir *d, int x, int y, struct board *b)
{
	int i;
	d->type = DIR_RIGHT;
	d->y = y;
	memset(d->pos, 0, sizeof(int) * BOARD_SIZE);
	d->pos[x] = 1;
	for (i = x; i >= 0 && b->tile[y][i].type != TILE_NONE; i--)
		d->x = i;
	for (i = x; i < BOARD_X && b->tile[y][i].type != TILE_NONE; i++)
		d->length = i;
	d->length -= d->x - 1;
	if (d->length == 1)
		 d->type = DIR_INVALID;
}


void mk_down(struct dir *d, int x, int y, struct board *b)
{
	int i;
	d->type = DIR_DOWN;
	d->x = x;
	memset(d->pos, 0, sizeof(int) * BOARD_SIZE);
	d->pos[y] = 1;
	for (i = y; i >= 0 && b->tile[i][x].type != TILE_NONE; i--)
		d->y = i;
	for (i = y; i < BOARD_Y && b->tile[i][x].type != TILE_NONE; i++)
		d->length = i;
	d->length -= d->y - 1;
	if (d->length == 1)
		 d->type = DIR_INVALID;
}


void mk_dot(struct action *a, struct move *m)
{
	int x = m->data.place.coor[0].x;
	int y = m->data.place.coor[0].y;
	struct path *path = &a->data.place.path; 
	struct board *b = &a->data.place.path.board;
	struct dir *d = NULL;
	path->type = PATH_DOT;
	d = &path->data.dot.right;
	mk_right(d, x, y, b);
	d = &path->data.dot.down;
	mk_down(d, x, y, b);
}


void mk_horz(struct action *a, struct move *m)
{
	int i;
	int x = m->data.place.coor[0].x;
	int y = m->data.place.coor[0].y;
	struct path *path = &a->data.place.path; 
	struct board *b = &a->data.place.path.board;
	struct dir *d = NULL;
	path->type = PATH_HORZ;
	d = &path->data.horz.right;
	mk_right(d, x, y, b);
	for (i = 0; i < m->data.place.num; i++) {
		x = m->data.place.coor[i].x;
		d->pos[x] = 1;
	}
	for (i = 0; i < BOARD_X; i++)
		path->data.horz.down[i].type = DIR_INVALID;
	for (i = 0; i < m->data.place.num; i++) {
		d = &path->data.horz.down[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mk_down(d, x, y, b);
	}
}


void mk_vert(struct action *a, struct move *m)
{
	int i;
	int x = m->data.place.coor[0].x;
	int y = m->data.place.coor[0].y;
	struct path *path = &a->data.place.path; 
	struct board *b = &a->data.place.path.board;
	struct dir *d = NULL;
	path->type = PATH_VERT;
	d = &path->data.vert.down;
	mk_down(d, x, y, b);
	for (i = 0; i < m->data.place.num; i++) {
		y = m->data.place.coor[i].y;
		d->pos[y] = 1;
	}
	for (i = 0; i < BOARD_Y; i++)
		path->data.vert.right[i].type = DIR_INVALID;
	for (i = 0; i < m->data.place.num; i++) {
		d = &path->data.vert.right[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mk_right(d, x, y, b);
	}
}


void mk_place(struct action *a, struct game *g, struct move *m)
{
	/* copy current board and recently placed tiles
	 * note: not-TILE_NONE impiles TILE_WILD or TILE_LETTER */
	struct path *path = &a->data.place.path;
	a->type = ACTION_PLACE;
	a->data.place.num = m->data.place.num;
	memcpy(&path->board, &g->board, sizeof(struct board));
	if (cpy_rack_board(&path->board, &m->data.place,
			&g->player[m->player_id])) {
		a->type = ACTION_INVALID;
		return;
	}
	switch (m->data.place.num) {
	case 0:
		a->type = ACTION_INVALID;
		return;
	case 1:
		mk_dot(a, m);
		break;
	default:
		if (is_horz(a, m)) {
			mk_horz(a, m);
		} else if (is_vert(a, m)) {
			mk_vert(a, m);
		} else {
			a->type = ACTION_INVALID;
			return;
		}
		break;
	}
	if (!valid_path(path, &g->dictionary)) {
		a->type = ACTION_INVALID;
		return;
	}
	a->data.place.score = score_path(path);
}


void mk_action(struct action *a, struct game *g, struct move *m)
{
	a->player_id = m->player_id;
	switch (m->type) {
	case MOVE_INVALID:
		a->type = ACTION_INVALID;
		break;
	case MOVE_PLACE:
		mk_place(a, g, m);
		break;
	case MOVE_SWAP:
		a->type = ACTION_INVALID;
		if (bag_size(&g->bag) >= m->data.swap.num) {
			a->type = ACTION_SWAP;
			memcpy(&a->data.swap, &m->data.swap,
					sizeof(struct swap));
		}
		break;
	case MOVE_SKIP:
		a->type = ACTION_SKIP;
		break;
	default:break;
	}
}


void apply_action(struct game *g, struct action *a)
{
	int pid = a->player_id;
	if (pid != g->turn)
		return;
	switch (a->type) {
	case ACTION_PLACE: {
		int i, r;
		memcpy(&g->board, &a->data.place.path.board,
				sizeof(struct board));
		g->player[pid].score += a->data.place.score;
		for (i = 0; i < a->data.place.num; i++) {
			r = a->data.place.rack_id[i];
			g->player[pid].tile[r].type = TILE_NONE;
		}
		break;
	}
	case ACTION_SWAP: {
		struct tile *t;
		int i, r;
		for (i = 0; i < a->data.swap.num; i++) {
			r = a->data.swap.rack_id[i];
			t = &g->player[pid].tile[r];
			bag_add(&g->bag, *t);
			*t = bag_peek(&g->bag);
			bag_drop(&g->bag);
		}
		shake_bag(&g->bag);
		break;
	}
	case ACTION_SKIP:
	default: break;
	}
}


void next_turn(struct game *g)
{
	g->turn ++;
	g->turn %= g->player_num;
}


