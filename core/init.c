#include <stdio.h>
#include <ctype.h>

#include "common.h"


void init_board(struct board *b)
{
	int x, y;

	NOT(b);

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			b->tile[y][x].type = TILE_NONE;
			b->sq[y][x] = SQ_NORMAL;
			if ((x == y) || (BOARD_Y - y - 1 == x)) {
				b->sq[y][x] = SQ_DBL_WRD;
			}
			if ((y % 7 == 0) && (x % 7 == 0)) {
				b->sq[y][x] = SQ_TRP_WRD;
			}
			if ((x % 4 == 1) && (y % 4 == 1)) {
				b->sq[y][x] = SQ_TRP_LET;
			}
			
		}
	}
	b->sq[1][1] = SQ_DBL_WRD;
	b->sq[13][1] = SQ_DBL_WRD;
	b->sq[1][13] = SQ_DBL_WRD;
	b->sq[13][13] = SQ_DBL_WRD;
	b->sq[7][7] = SQ_FREE;

	b->sq[14][3] = SQ_DBL_LET;
	b->sq[3][14] = SQ_DBL_LET;
	b->sq[0][3] = SQ_DBL_LET;
	b->sq[3][0] = SQ_DBL_LET;
	b->sq[0][11] = SQ_DBL_LET;
	b->sq[11][0] = SQ_DBL_LET;
	b->sq[14][11] = SQ_DBL_LET;
	b->sq[11][14] = SQ_DBL_LET;
	b->sq[2][8] = SQ_DBL_LET;
	b->sq[8][2] = SQ_DBL_LET;
	b->sq[2][6] = SQ_DBL_LET;
	b->sq[6][2] = SQ_DBL_LET;
	b->sq[3][7] = SQ_DBL_LET;
	b->sq[7][3] = SQ_DBL_LET;
	b->sq[8][2] = SQ_DBL_LET;
	b->sq[2][8] = SQ_DBL_LET;
	b->sq[12][6] = SQ_DBL_LET;
	b->sq[6][12] = SQ_DBL_LET;
	b->sq[11][7] = SQ_DBL_LET;
	b->sq[7][11] = SQ_DBL_LET;
	b->sq[12][8] = SQ_DBL_LET;
	b->sq[8][12] = SQ_DBL_LET;
	b->sq[8][8] = SQ_DBL_LET;
	b->sq[6][6] = SQ_DBL_LET;
	b->sq[6][8] = SQ_DBL_LET;
	b->sq[8][6] = SQ_DBL_LET;
}

const int tile_letter_num[LETTER_COUNT] = 
	{ 9	/* A */
	, 2	/* B */
	, 2	/* C */
	, 4	/* D */
	, 12	/* E */
	, 2	/* F */
	, 3	/* G */
	, 2	/* H */
	, 9	/* I */
	, 1	/* J */
	, 1	/* K */
	, 4	/* L */
	, 2	/* M */
	, 6	/* N */
	, 8	/* O */
	, 2	/* P */
	, 1	/* Q */
	, 6	/* R */
	, 4	/* S */
	, 6	/* T */
	, 4	/* U */
	, 2	/* V */
	, 2	/* W */
	, 1	/* X */
	, 2	/* Y */
	, 1	/* Z */
	};

const int tile_wild_num = 2;

void shake_bag(struct bag *b, int offset)
{
	int i, j;
	int val[BAG_SIZE];

	NOT(b);

	srand(offset);
	for (i = 0; i < BAG_SIZE; i++) {
		val[i] = rand();
	}
	/* quick and dirty - bubble sort :P */
	i = b->head;
	while (i != b->tail) {
		j = i;
		while (j != b->tail) {
			if (val[i] > val[j]) {
				struct tile tmp = b->tile[i];
				int tmp_v = val[i];
				val[i] = val[j];
				val[j] = tmp_v;
				
				b->tile[i] = b->tile[j];
				b->tile[j] = tmp;
			}
			j++;
			j %= BAG_SIZE;
		}
		i++;
		i %= BAG_SIZE;
	}
	
}

void init_bag(struct bag *b)
{
	int i, j, k;

	NOT(b);

	b->head = 0;
	b->tail = BAG_SIZE - 1;
	for (i = 0; i < BAG_SIZE; i++) {
		b->tile[i].type = TILE_LETTER;
	}
	i = 0;
	for (k = 0; k < LETTER_COUNT; k++) {
		for (j = 0; j < tile_letter_num[k]; j++) {
			b->tile[i].letter = LETTER_A + k;
			i++;
		}
	}
	for (i = 0; i < tile_wild_num; i++) {
		b->tile[i].type = TILE_WILD;
	}
	shake_bag(b, 0);
}


void init_player(struct player *p, struct bag *b)
{
	int i;

	NOT(p);

	for (i = 0; i < RACK_SIZE; i++) {
		p->tile[i].type = TILE_NONE;
	}
	refill_rack(p, b);

	p->score = 0;
	p->active = true;
}


void init_move(struct move *m)
{
	int i;
	
	NOT(m);

	m->type = MOVE_PLACE;
	m->data.place.num = 4;
	for (i = 0; i < RACK_SIZE; i++) {
		m->data.place.rack_id[i] = i;
		m->data.place.coor[i].x = 0;
		m->data.place.coor[i].y = 0;
	}
	m->data.place.coor[0].y = 0;
	m->data.place.coor[1].y = 1;
	m->data.place.coor[2].y = 2;
	m->data.place.coor[3].y = 3;
}


void cons_word(struct word *w, const char *str)
{
	int i;
	char c;
	
	NOT(w), NOT(str);
	
	for (i = 0, w->len = 0; str[i] != '\0' && w->len < BOARD_SIZE; i++) {
		c = toupper(str[i]);
		if (c >= 'A' && c <= 'Z') {
			w->letter[w->len] = LETTER_A + c - 'A';
			w->len++;
		}
	}
}


void swap_word(struct word *w0, struct word *w1)
{
	struct word tmp;

	NOT(w0), NOT(w1);
	
	tmp = *w0;
	*w0 = *w1;
	*w1 = tmp;
}


int cmp_word_wrapper(const void *p0, const void *p1)
{
	NOT(p0), NOT(p1);

	return cmp_word((struct word*)p0, (struct word*)p1);
}


bool init_dict(struct dict *d, const char *name)
{
	long i;
	FILE *f = NULL;
	struct word w;
	char buf[BOARD_SIZE + 1];
	
	NOT(d), NOT(name);
	
	f = fopen(name, "r");
	if (f == NULL) {
		return false;
	}
	/* count */
	d->num = 0;
	while (fgets(buf, BOARD_SIZE + 1, f)) {
		cons_word(&w, buf);
		if (w.len > 1) {
			d->num++;
		}
	}
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return false;
	}
	rewind(f);
	assert(d->num > 0);
	/* alloc */
	d->words = alloc_mem(sizeof(struct word) * d->num);
	NOT(d->words);
	i = 0;
	for (i = 0; i < d->num && fgets(buf, BOARD_SIZE + 1, f); i++) {
		cons_word(&w, buf);
		if (w.len > 1) {
			d->words[i] = w;
		} else {
			i--;
		}
	}
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return false;
	}
	fclose(f);
	/* sort */
	qsort(d->words, d->num, sizeof(struct word), cmp_word_wrapper);
	return true;
}


void quit_dict(struct dict *dict)
{
	NOT(dict);
	free_mem(dict->words);
}

