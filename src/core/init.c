#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "common.h"


void boardInit(Board *b)
{
	int x, y;

	NOT(b);
	assert(BOARD_X == 15);
	assert(BOARD_Y == 15);

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			b->tile[y][x].type = tileNone;
			b->sq[y][x] = sqNormal;
			if ((x == y) || (BOARD_Y - y - 1 == x)) {
				b->sq[y][x] = sqDblWrd;
			}
			if ((y % 7 == 0) && (x % 7 == 0)) {
				b->sq[y][x] = sqTrpWrd;
			}
			if ((x % 4 == 1) && (y % 4 == 1)) {
				b->sq[y][x] = sqTrpLet;
			}
			
		}
	}
	b->sq[1][1] = sqDblWrd;
	b->sq[13][1] = sqDblWrd;
	b->sq[1][13] = sqDblWrd;
	b->sq[13][13] = sqDblWrd;
	b->sq[7][7] = sqFree;

	b->sq[14][3] = sqDblLet;
	b->sq[3][14] = sqDblLet;
	b->sq[0][3] = sqDblLet;
	b->sq[3][0] = sqDblLet;
	b->sq[0][11] = sqDblLet;
	b->sq[11][0] = sqDblLet;
	b->sq[14][11] = sqDblLet;
	b->sq[11][14] = sqDblLet;
	b->sq[2][8] = sqDblLet;
	b->sq[8][2] = sqDblLet;
	b->sq[2][6] = sqDblLet;
	b->sq[6][2] = sqDblLet;
	b->sq[3][7] = sqDblLet;
	b->sq[7][3] = sqDblLet;
	b->sq[8][2] = sqDblLet;
	b->sq[2][8] = sqDblLet;
	b->sq[12][6] = sqDblLet;
	b->sq[6][12] = sqDblLet;
	b->sq[11][7] = sqDblLet;
	b->sq[7][11] = sqDblLet;
	b->sq[12][8] = sqDblLet;
	b->sq[8][12] = sqDblLet;
	b->sq[8][8] = sqDblLet;
	b->sq[6][6] = sqDblLet;
	b->sq[6][8] = sqDblLet;
	b->sq[8][6] = sqDblLet;

/*
	for (y = 0; y <= 7; y++) {
		for (x = 0; x <= 7; x++) {
			b->sq[y][x] = sqNormal;
			if (x == y && x == 0) {
				b->sq[y][x] = sqDblLet;
				continue;
			}
			if (((x == y) && (x == 2 || x == 7)) || (x + y == 5 && x < 4 && y < 4)) {
				b->sq[y][x] = sqBlock;
				continue;
			}
			if (x > 0 && y > 0 && x + y == 3) {
				b->sq[y][x] = sqFree;
				continue;
			}
			if ((x == 6 && y == 5 ) || (x == 5 && y == 6)) {
				b->sq[y][x] = sqNoVowel;
				continue;
			}
			if (((x == 0 || x == 7) && (y == 0 || y == 7)) || (x == 6 && y == 7) || (x == 7 && y == 6)) {
				b->sq[y][x] = sqTrpWrd;
				continue;
			}
			if (x < 6 && y < 6 && x + y == 7) {
				b->sq[y][x] = sqDblWrd;
				continue;
			}
			if ((x == 3 || x == 7)&& (y == 3 || y == 7)) {
				b->sq[y][x] = sqTrpLet;
				continue;
			}
			if ((x % 3 == 2  && y % 3 == 2) || (x % 5 == 1 && y % 5 == 1)) {
				b->sq[y][x] = sqDblLet;
				continue;
			}
			
		}
	}

	for (y = 0; y <= 7; y++) {
		for (x = 7; x < 15; x++) {
			b->sq[y][x] = b->sq[y][14-x];
		}
	}
	for (y = 7; y < 15; y++) {
		for (x = 15; x >= 7; x--) {
			b->sq[y][x] = b->sq[14-y][14-x];
		}
	}
	for (y = 7; y < 15; y++) {
		for (x = 0; x <= 7; x++) {
			b->sq[y][x] = b->sq[14-y][x];
		}
	}

	for (x = 4; x < 7; x++) {
		b->sq[13][x] = sqBlock;
		b->sq[1][14-x] = sqBlock;
	}

	b->sq[7][7] = sqBlock;
	
	b->sq[7][3]  = sqNoVowel;
	b->sq[7][11] = sqNoVowel;

*/

	b->sq[1][1] = sqDblWrd;
	b->sq[13][1] = sqDblWrd;
	b->sq[1][13] = sqDblWrd;
	b->sq[13][13] = sqDblWrd;
	b->sq[7][7] = sqFree;

	b->sq[3][14] = sqDblLet;
	b->sq[0][3] = sqDblLet;
	b->sq[3][0] = sqDblLet;
	b->sq[0][11] = sqDblLet;
	b->sq[11][0] = sqDblLet;
	b->sq[14][11] = sqDblLet;
	b->sq[11][14] = sqDblLet;
	b->sq[2][8] = sqDblLet;
	b->sq[8][2] = sqDblLet;
	b->sq[2][6] = sqDblLet;
	b->sq[6][2] = sqDblLet;
	b->sq[3][7] = sqDblLet;
	b->sq[7][3] = sqDblLet;
	b->sq[8][2] = sqDblLet;
	b->sq[2][8] = sqDblLet;
	b->sq[12][6] = sqDblLet;
	b->sq[6][12] = sqDblLet;
	b->sq[11][7] = sqDblLet;
	b->sq[7][11] = sqDblLet;
	b->sq[12][8] = sqDblLet;
	b->sq[8][12] = sqDblLet;
	b->sq[8][8] = sqDblLet;
	b->sq[6][6] = sqDblLet;
	b->sq[6][8] = sqDblLet;
	b->sq[8][6] = sqDblLet;
	b->sq[8][6] = sqDblLet;
}


void bagShake(Bag *b, int offset)
{
	int i, j;
	int val[BAG_SIZE];

	NOT(b);

	srand(offset);
	for (i = 0; i < BAG_SIZE; i++) {
		val[i] = rand();
	}

	/* quick and dirty, bubble sort */
	i = b->head;
	while (i != b->tail) {
		j = i;
		while (j != b->tail) {
			if (val[i] > val[j]) {
				Tile tmp = b->tile[i];
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

void bagInit(Bag *b)
{
	int i, j, k;

	static const int tileLetterNum[letterCount] = {
		9,	/* A */
		2,	/* B */
		2,	/* C */
		4,	/* D */
		12,	/* E */
		2,	/* F */
		3,	/* G */
		2,	/* H */
		9,	/* I */
		1,	/* J */
		1,	/* K */
		4,	/* L */
		2,	/* M */
		6,	/* N */
		8,	/* O */
		2,	/* P */
		1,	/* Q */
		6,	/* R */
		4,	/* S */
		6,	/* T */
		4,	/* U */
		2,	/* V */
		2,	/* W */
		1,	/* X */
		2,	/* Y */
		1	/* Z */
	};

	static const int tileWildNum = 2;

	NOT(b);

	b->head = 0;
	b->tail = BAG_SIZE - 1;
	for (i = 0; i < tileWildNum; i++) {
		b->tile[i].type = tileWild;
		b->tile[i].letter = letterA;
	}
	for (k = 0, i = 2; k < letterCount; k++) {
		for (j = 0; j < tileLetterNum[k]; j++, i++) {
			b->tile[i].type = tileLetter;
			b->tile[i].letter = letterA + k;
		}
	}
	for (; i < BAG_SIZE; i++) {
		b->tile[i].type = tileNone;
	}

	/* short for testing */
	/*b->tail = 20; bagShake(b, 0);*/
	bagShake(b, time(NULL));
}


void playerInit(Player *p, Bag *b)
{
	int i;

	NOT(p);

	for (i = 0; i < RACK_SIZE; i++) {
		p->tile[i].type = tileNone;
		p->tile[i].letter = letterA;
	}
	rackRefill(p, b);

	p->score = 0;
	p->active = true;
}

void initPlayerHuman(Player *p, Bag *b)
{
	NOT(p);
	NOT(b);

	playerInit(p, b);
	p->type = playerHuman;
}

void initPlayerAI(Player *p, Bag *b)
{
	NOT(p);
	NOT(b);

	playerInit(p, b);
	p->type = playerAI;
	p->aiShare.shareStart = false;
	p->aiShare.shareEnd = false;
	p->aiShare.loading = 0;
	p->aiShare.action.type = actionInvalid;
}

void moveInit(Move *m)
{
	int i;
	
	NOT(m);

	m->type = movePlace;
	m->data.place.num = 4;
	for (i = 0; i < RACK_SIZE; i++) {
		m->data.place.rackIdx[i] = i;
		m->data.place.coor[i].x = 0;
		m->data.place.coor[i].y = 0;
	}
	m->data.place.coor[0].y = 0;
	m->data.place.coor[1].y = 1;
	m->data.place.coor[2].y = 2;
	m->data.place.coor[3].y = 3;
}


void wordCons(Word *w, const char *str)
{
	int i;
	char c;
	
	NOT(w);
	NOT(str);
	
	for (i = 0, w->len = 0; str[i] != '\0' && w->len < BOARD_SIZE; i++) {
		c = toupper(str[i]);
		if (c >= 'A' && c <= 'Z') {
			w->letter[w->len] = letterA + c - 'A';
			w->len++;
		}
	}
}


void swapWord(Word *w0, Word *w1)
{
	Word tmp;

	NOT(w0);
	NOT(w1);
	
	tmp = *w0;
	*w0 = *w1;
	*w1 = tmp;
}


int cmpWordWrapper(const void *p0, const void *p1)
{
	NOT(p0);
	NOT(p1);

	return cmpWord((Word*)p0, (Word*)p1);
}

bool dictInit(Dict *d, const char *name)
{
	long i;
	FILE *f = NULL;
	Word w;
	char buf[BOARD_SIZE + 1];
	
	NOT(d);
	NOT(name);
	
	f = fopen(name, "r");
	if (f == NULL) {
		return false;
	}
	/* count */
	d->num = 0;
	while (fgets(buf, BOARD_SIZE + 1, f)) {
		wordCons(&w, buf);
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
	d->words = memAlloc(sizeof(Word) * d->num);

	NOT(d->words);

	i = 0;
	for (i = 0; i < d->num && fgets(buf, BOARD_SIZE + 1, f); i++) {
		wordCons(&w, buf);
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
	qsort(d->words, d->num, sizeof(Word), cmpWordWrapper);
	return true;
}

bool dictInitCount7(Dict *d, float *count, float increase, const char *name)
{
	long i;
	FILE *f = NULL;
	Word w;
	char buf[BOARD_SIZE + 1];
	
	NOT(d);
	NOT(name);
	
	f = fopen(name, "r");
	if (f == NULL) {
		return false;
	}
	*count += increase;
	/* count */
	d->num = 0;
	while (fgets(buf, BOARD_SIZE + 1, f)) {
		wordCons(&w, buf);
		if (w.len > 1) {
			d->num++;
		}
	}
	*count += increase;
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return false;
	}
	rewind(f);
	*count += increase;
	assert(d->num > 0);
	/* alloc */
	d->words = memAlloc(sizeof(Word) * d->num);
	*count += increase;

	NOT(d->words);

	i = 0;
	for (i = 0; i < d->num && fgets(buf, BOARD_SIZE + 1, f); i++) {
		wordCons(&w, buf);
		if (w.len > 1) {
			d->words[i] = w;
		} else {
			i--;
		}
	}
	*count += increase;
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return false;
	}
	fclose(f);
	/* sort */
	*count += increase;
	qsort(d->words, d->num, sizeof(Word), cmpWordWrapper);
	*count += increase;
	return true;
}


void dictQuit(Dict *dict)
{
	NOT(dict);
	if (dict->words) {
		memFree(dict->words);
	}
}

