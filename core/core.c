#include "common.h"

void print_word(struct word *w);

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
	int p, i, t;

	NOT(b), NOT(d);

	dw = 0;
	tw = 0;
	x = d->x;
	y = d->y;
	score = 0;
	switch (d->type) {
	case DIR_RIGHT: {
		for (p = 0, i = d->x; i < d->len + d->x; p++, i++) {
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
		for (p = 0, i = d->y; i < d->len + d->y; p++, i++) {
			t   = tile_score(&b->tile[i][x]);
			t  *= can_use_dbl_let(b, d, p, x, i) ? 2 : 1;
			t  *= can_use_trp_let(b, d, p, x, i) ? 3 : 1;
			dw += can_use_dbl_wrd(b, d, p, x, i);
			tw += can_use_trp_wrd(b, d, p, x, i);
			score += t;
		}
		break;
	}
	case DIR_INVALID: /* fall through */
	default: return 0;
	}
	if (dw > 0) {
		score *= dw * 2;
	}
	if (tw > 0) {
		score *= tw * 3;
	}
	return score;
}


int score_meta_path(struct dir *d, struct dir *adj, int n, struct board *b)
{
	int i, s;

	NOT(d), NOT(adj), NOT(b);

	s = 0;
	if (d->type != DIR_INVALID) {
		s = score_dir(b, d);
		for (i = 0; i < n; i++) {
			if (adj[i].type != DIR_INVALID) {
				s += score_dir(b, adj+i);
			}
		}
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
	case PATH_DOT: {
		if (p->data.dot.right.type == DIR_RIGHT) {
			score  = score_dir(b, &p->data.dot.right);
		}
		if (p->data.dot.down.type == DIR_DOWN) {
			score += score_dir(b, &p->data.dot.down);
		}
		break;
	}
	case PATH_HORZ: {
		score = score_meta_path(&p->data.horz.right, p->data.horz.down, BOARD_X, b);
		break;
	}
	case PATH_VERT: {
		score = score_meta_path(&p->data.vert.down, p->data.vert.right, BOARD_Y, b);
		break;
	}
	case PATH_INVALID: /* fall through */
	default: return 0;
	}
	return score;
}


bool bag_full(struct bag *b)
{
	NOT(b);

	return (b->head + 1) % BAG_SIZE == b->tail;
}


bool bag_empty(struct bag *b)
{
	NOT(b);

	return b->head == b->tail;
}


int bag_size(struct bag *b)
{
	NOT(b);

	if (b->head > b->tail) {
		return b->head - b->tail;
	}
	return b->tail - b->head;
}


struct tile bag_peek(struct bag *b)
{
	struct tile next;

	NOT(b);

	next.type = TILE_NONE;
	if (b->head != b->tail) {
		next = b->tile[b->head];
	}
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


cmp_t cmp_word(struct word *w0, struct word *w1) 
{
	/* w0 >  w1 -> -1
	 * w0 <  w1 ->  1
	 * w0 == w1 ->  0
	 */
	int i;

	NOT(w0), NOT(w1);

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


bool valid_word(struct word *w, struct dict *d)
{
	long min, max, mid;
	cmp_t res;
	
	NOT(w), NOT(d);
	
	min = 0;
	max = d->num;
	mid = d->num / 2;
	while (min <= max) {
		res = cmp_word(w, &d->words[mid]);
		switch (res) {
		case   CMP_EQUAL: return true;
		case CMP_GREATER: min = mid + 1; break;
		case    CMP_LESS: max = mid - 1; break;
		default: return false; /* Should never arrive here via cmp_word */
		}
		mid = (min + max) / 2;
	}
	return false;
}


bool valid_dir(struct dir *dir, struct board *b, struct dict *dict)
{
	int x, y, i;
	struct word w;

	NOT(dir), NOT(b), NOT(dict);

	x = dir->x;
	y = dir->y;
	w.len = dir->len;
	switch (dir->type) {
	case DIR_RIGHT: {
		for (i = 0; i < w.len; i++) {
			if (b->tile[y][x + i].type != TILE_NONE) {
				w.letter[i] = b->tile[y][x + i].letter;
			} else {
				return false;
			}
		}
		break;
	}
	case DIR_DOWN: {
		for (i = 0; i < w.len; i++) {
			if (b->tile[y + i][x].type != TILE_NONE) {
				w.letter[i] = b->tile[y + i][x].letter;
			} else {
				return false;
			}
		}
		break;
	}
	case DIR_INVALID: /* fall through */
	default: return false;
	}
	return valid_word(&w, dict);
}


bool valid_path(struct path *p, struct dict *d)
{
	int i;

	NOT(p), NOT(d);

	switch (p->type) {
	case PATH_DOT: {
		if (!valid_dir(&p->data.dot.right, &p->board, d)) {
			return false;
		}
		if (!valid_dir(&p->data.dot.down, &p->board, d)) {
			return false;
		}
		break;
	}
	case PATH_HORZ: {
		if (!valid_dir(&p->data.horz.right, &p->board, d)) {
			return false;
		}
		for (i = 0; i < BOARD_X; i++) {
			if (p->data.horz.down[i].type == DIR_DOWN &&
			   (!valid_dir(&p->data.horz.down[i], &p->board, d))) {
				return false;
			}
		}
		break;
	}
	case PATH_VERT: {
		if (!valid_dir(&p->data.vert.down, &p->board, d)) {
			return false;
		}
		for (i = 0; i < BOARD_Y; i++) {
			if (p->data.vert.right[i].type == DIR_RIGHT && 
			   (!valid_dir(&p->data.vert.right[i], &p->board, d))) {
				return false;
			}
		}
		break;
	}
	case PATH_INVALID: /* fall through */
	default: return false;
	}
	return true;
}


bool adjacent_tiles(struct board *b, struct place *place, struct player *player)
{
	int x, y, i, r;

	NOT(b), NOT(place), NOT(player);

	for (i = 0; i < place->num; i++) {
		r = place->rack_id[i];
		y = place->coor[i].y;
		x = place->coor[i].x;
		if (player->tile[r].type != TILE_NONE) {
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


bool on_free_squares(struct board *b, struct place *place, struct player *player)
{
	int x, y, i, r;

	NOT(b), NOT(place), NOT(player);

	for (i = 0; i < place->num; i++) {
		r = place->rack_id[i];
		y = place->coor[i].y;
		x = place->coor[i].x;
		if (player->tile[r].type != TILE_NONE &&
				b->sq[y][x] == SQ_FREE) {
			return true;
		}
	}
	return false;
}


bool place_in_range(struct place *p)
{
	int i;
	
	NOT(p);
	
	for (i = 0; i < p->num; i++) {
		if (p->rack_id[i] < 0 || p->rack_id[i] >= RACK_SIZE) {
			return false;
		}
		if (p->coor[i].x < 0 || p->coor[i].x >= BOARD_X) {
			return false;
		}
		if (p->coor[i].y < 0 || p->coor[i].y >= BOARD_Y) {
			return false;
		}
	}
	return true;
}


bool place_overlap(struct place *p)
{
	int i, j;

	NOT(p);
	
	for (i = 0; i < p->num; i++) {
		for (j = i + 1; j < p->num; j++) {
			if (p->rack_id[i] == p->rack_id[j]) {
				return false;
			}
			if (p->coor[i].x == p->coor[j].x &&
			    p->coor[i].y == p->coor[j].y) {
				return true;
			}
		}
	}
	return false;
}


bool place_overlap_board(struct place *p, struct board *b)
{
	int i, x, y;

	NOT(p), NOT(b);
	
	for (i = 0; i < p->num; i++) {
		x = p->coor[i].x;
		y = p->coor[i].y;
		if (b->tile[y][x].type != TILE_NONE) {
			return true;
		}
	}
	return false;
}


bool place_rack_exist(struct place *place, struct player *player)
{
	int i, r;

	NOT(place), NOT(player);

	for (i = 0; i < place->num; i++) {
		r = place->rack_id[i];
		if (player->tile[r].type == TILE_NONE) {
			return false;
		}
	}
	return true;
}


void cpy_rack_board(struct board *b, struct place *place, struct player *player)
{
	int x, y, i, r;

	NOT(b), NOT(place), NOT(player);

	for (i = 0; i < place->num; i++) {
		r = place->rack_id[i];
		y = place->coor[i].y;
		x = place->coor[i].x;
		cpy_mem(&b->tile[y][x], &player->tile[r],
				sizeof(struct tile));
	}
}


bool is_horz(struct action *a, struct move *m)
{
	int i, min, max, y;
	struct coor *coor;
	struct board *b;

	NOT(a), NOT(m);

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


bool is_vert(struct action *a, struct move *m)
{
	int i, min, max, x;
	struct coor *coor;
	struct board *board;
	

	NOT(a), NOT(m);

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


void mk_right(struct dir *d, int x, int y, struct board *b)
{
	int i;

	NOT(d), NOT(b), VALID_BOARD_X(x), VALID_BOARD_Y(y);

	d->type = DIR_RIGHT;
	d->y = y;
	set_mem(d->pos, 0, sizeof(int) * BOARD_SIZE);
	d->pos[x] = 1;
	for (i = x; i >= 0 && b->tile[y][i].type != TILE_NONE; i--) {
		d->x = i;
	}
	for (i = x; i < BOARD_X && b->tile[y][i].type != TILE_NONE; i++) {
		d->len = i;
	}
	d->len -= d->x - 1;
	if (d->len == 1) {
		 d->type = DIR_INVALID;
	}
}


void mk_down(struct dir *d, int x, int y, struct board *b)
{
	int i;

	NOT(d), NOT(b), VALID_BOARD_X(x), VALID_BOARD_Y(y);

	d->type = DIR_DOWN;
	d->x = x;
	set_mem(d->pos, 0, sizeof(int) * BOARD_SIZE);
	d->pos[y] = 1;
	for (i = y; i >= 0 && b->tile[i][x].type != TILE_NONE; i--) {
		d->y = i;
	}
	for (i = y; i < BOARD_Y && b->tile[i][x].type != TILE_NONE; i++) {
		d->len = i;
	}
	d->len -= d->y - 1;
	if (d->len == 1) {
		 d->type = DIR_INVALID;
	}
}


void mk_dot(struct action *a, struct move *m)
{
	int x, y;
	struct path *p;
	struct board *b;
	struct dir *d;

	NOT(a), NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	p = &a->data.place.path; 
	b = &a->data.place.path.board;
	d = NULL;
	p->type = PATH_DOT;
	d = &p->data.dot.right;
	mk_right(d, x, y, b);
	d = &p->data.dot.down;
	mk_down(d, x, y, b);
}


void mk_horz(struct action *a, struct move *m)
{
	int i, x, y;
	struct path *p;
	struct board *b;
	struct dir *d;

	NOT(a), NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	p = &a->data.place.path; 
	b = &a->data.place.path.board;
	p->type = PATH_HORZ;
	d = &p->data.horz.right;
	mk_right(d, x, y, b);
	for (i = 0; i < m->data.place.num; i++) {
		x = m->data.place.coor[i].x;
		d->pos[x] = 1;
	}
	for (i = 0; i < BOARD_X; i++) {
		p->data.horz.down[i].type = DIR_INVALID;
	}
	for (i = 0; i < m->data.place.num; i++) {
		d = &p->data.horz.down[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mk_down(d, x, y, b);
	}
}


void mk_vert(struct action *a, struct move *m)
{
	int i, x, y;
	struct path *path;
	struct board *b;
	struct dir *d;

	NOT(a), NOT(m);

	x = m->data.place.coor[0].x;
	y = m->data.place.coor[0].y;
	b = &a->data.place.path.board;
	path = &a->data.place.path; 
	d = &path->data.vert.down;
	path->type = PATH_VERT;
	mk_down(d, x, y, b);
	for (i = 0; i < m->data.place.num; i++) {
		y = m->data.place.coor[i].y;
		d->pos[y] = 1;
	}
	for (i = 0; i < BOARD_Y; i++) {
		path->data.vert.right[i].type = DIR_INVALID;
	}
	for (i = 0; i < m->data.place.num; i++) {
		d = &path->data.vert.right[i];
		x = m->data.place.coor[i].x;
		y = m->data.place.coor[i].y;
		mk_right(d, x, y, b);
	}
}


action_err_t fd_place_err
		(struct place *place, struct player *player, struct board *board)
{
	NOT(place), NOT(player), NOT(board);
	
	if (!place_in_range(place)) {
		return ACTION_ERR_PLACE_OUT_OF_RANGE;
	}
	if (place_overlap(place)) {
		return ACTION_ERR_PLACE_SELF_OVERLAP;
	}
	if (place_overlap_board(place, board)) {
		return ACTION_ERR_PLACE_BOARD_OVERLAP;
	}
	if (!place_rack_exist(place, player)) {
		return ACTION_ERR_PLACE_INVALID_RACK_ID;
	}
	if (!adjacent_tiles(board, place, player) &&
	    !on_free_squares(board, place, player)) {
		return ACTION_ERR_PLACE_INVALID_SQ;
	}
	return ACTION_ERR_NONE;
}


void mk_place(struct action *a, struct game *g, struct move *m)
{
	int num, i;
	struct path *path;
	struct player *player;
	action_err_t err;

	NOT(a), NOT(g), NOT(m);

	num = m->data.place.num;
	path = &a->data.place.path;
	player = &g->player[m->player_id];
	a->type = ACTION_PLACE;

	a->data.place.num = m->data.place.num;
	for (i = 0; i < num; i++) {
		a->data.place.rack_id[i] = m->data.place.rack_id[i];
	}

	err = fd_place_err(&m->data.place, player, &g->board);
	if (err != ACTION_ERR_NONE) {
		a->type = ACTION_INVALID;
		a->data.err = err;
		return;
	}
	cpy_mem(&path->board, &g->board, sizeof(struct board));
	cpy_rack_board(&path->board, &m->data.place, player);
	if (num <= 0) {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_PLACE_NO_RACK;
		return;
	}
	if (num == 1) {
		mk_dot(a, m);
	} else if (num >= 2) {
		if (is_horz(a, m)) {
			mk_horz(a, m);
		} else if (is_vert(a, m)) {
			mk_vert(a, m);
		} else {
			a->type = ACTION_INVALID;
			a->data.err = ACTION_ERR_PLACE_NO_DIR;
			return;
		}
	}
	if (!valid_path(path, &g->dict)) {
		a->type = ACTION_INVALID;
		a->data.err = ACTION_ERR_PLACE_INVALID_PATH;
		return;
	}
	a->data.place.score = score_path(path);
}


void mk_discard(struct action *a, struct game *g, struct move *m)
{
	int i;

	NOT(a), NOT(g), NOT(m);

	a->type = ACTION_DISCARD;
	a->data.discard.num = m->data.discard.num;
	printf("[[%d]]\n", m->data.discard.num);
	for (i = 0; i < a->data.discard.num; i++) {
		printf("[%d]\n", a->data.discard.rack_id[i]);
		a->data.discard.rack_id[i] = m->data.discard.rack_id[i];
	}
	/* cpy_mem(&a->data.discard, &m->data.discard, sizeof(struct discard)); */
}


void mk_action(struct action *a, struct game *g, struct move *m)
{
	NOT(a), NOT(g), NOT(m);
	a->player_id = m->player_id;
	switch (m->type) {
	case MOVE_PLACE: mk_place(a, g, m); break;
	case MOVE_DISCARD: mk_discard(a, g, m); break;
	case MOVE_SKIP: a->type = ACTION_SKIP; break;
	case MOVE_QUIT: a->type = ACTION_QUIT; break;
	case MOVE_INVALID: /* fall through */
	default: a->type = ACTION_INVALID; break;
	}
}


void shift_rack(struct player *p)
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
		cpy_mem(&p->tile[j], &p->tile[i], sizeof(struct tile));
		i++;
		j++;
	}
	while (j < RACK_SIZE) {
		p->tile[j].type = TILE_NONE;
		j++;
	}
}


void refill_rack(struct player *p, struct bag *b)
{
	int i;
	
	NOT(p), NOT(b);
	
	for (i = 0; i < RACK_SIZE && !bag_empty(b); i++) {
		if (p->tile[i].type == TILE_NONE) {
			p->tile[i] = bag_peek(b);
			bag_drop(b);
		}
	}
}


bool apply_action(struct game *g, struct action *a)
{
	int id, i, r;

	NOT(g), NOT(a);

	id = a->player_id;
	if (id != g->turn) {
		return false;
	}
	switch (a->type) {
	case ACTION_PLACE: {
		cpy_mem(&g->board, &a->data.place.path.board,
				sizeof(struct board));
		g->player[id].score += a->data.place.score;
		for (i = 0; i < a->data.place.num; i++) {
			r = a->data.place.rack_id[i];
			g->player[id].tile[r].type = TILE_NONE;
		}
		refill_rack(&g->player[id], &g->bag);
		shift_rack(&g->player[id]);
		break;
	}
	case ACTION_DISCARD: {
		for (i = 0; i < a->data.discard.num; i++) {
			r = a->data.discard.rack_id[i];
			g->player[id].tile[r].type = TILE_NONE;
		}
		refill_rack(&g->player[id], &g->bag);
		shift_rack(&g->player[id]);
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


void remove_from_rack(struct player *p, int *rack_id, int n)
{
	int i;

	NOT(p), NOT(rack_id);

	for (i = 0; i < n; i++) {
		p->tile[rack_id[i]].type = TILE_NONE;
	}
}


void next_turn(struct game *g)
{
	NOT(g);

	do {
		g->turn ++;
		g->turn %= g->player_num;
	} while (!g->player[g->turn].active);
}


void clr_move(struct move *m)
{
	NOT(m);

	set_mem(m, 0, sizeof(struct move));
	m->type = MOVE_INVALID;
}


void clr_action(struct action *a)
{
	NOT(a);

	set_mem(a, 0, sizeof(struct action));
	a->type = ACTION_INVALID;
}


bool end_game(struct game *g)
{
	int i, j, k;

	NOT(g);

	/* at least 2 players are active */	
	j = 0;
	for (i = 0; i < g->player_num; i++) {
		if (g->player[i].active) {
			j++;
		}
	}
	if (j < 2) {
			return true;
	}
	/* at least 1 player has tiles on the rack */
	j = 0;
	for (i = 0; i < g->player_num; i++) {
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


int fd_winner(struct game *g)
{
	int max, id, i, j;

	NOT(g);

	id = -1;
	j = 0;
	for (i = 0; i < g->player_num; i++) {
		if (g->player[i].active) {
			id = i;
			j++;
		}
	} 
	if (j == 1) {
		return id;
	}
	max = 0;
	for (i = 0; i < g->player_num; i++) {
		if (g->player[i].active) {
			if (max < g->player[i].score) {
				id = i;
				max = g->player[i].score;
			}
		}
	} 
	return max ? id : -1;
}

