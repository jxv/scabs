#include <string.h>

#include "core.h"
#include "print.h"

void printLetter(letter_t l)
{
	assert(l >= LETTER_A);
	assert(l <= LETTER_Z);
	putchar('A' + l - LETTER_A);
}

void printWord(const word_t *w)
{
	char str[BOARD_SIZE];
	int j;

	NOT(w);

	for (j = 0; j < w->len; j++) {
		str[j] = 'A' + w->letter[j] - LETTER_A;
	}
	str[j] = '\0';
	printf("[%s]\n", str);
}

void printDict(const dict_t *d)
{
	int i;

	NOT(d);

	printf("== Size:%ld\n", d->num);
	for (i = 0; i < d->num; i++) {
		printWord(&d->words[i]);
	}
}

void printTile(const tile_t *t)
{
	char c;
	
	NOT(t);

	switch (t->type) {
	case TILE_WILD: c = 'a' + t->letter; break;
	case TILE_LETTER: c = 'A' + t->letter; break;
	default: return;
	}
	putchar(c);
}

void printBag(const bag_t *b)
{
	int i;

	NOT(b);

	i = b->head;
	while (i != b->tail) {
		printTile(&b->tile[i]);
		i++;
		i %= BAG_SIZE;
	}
	putchar('\n');
}

void printAction(const action_t *a)
{
	NOT(a);

	switch (a->type) {
	case ACTION_INVALID: printf("action invalid\n"); break;
	case ACTION_PLACE: {
		printf("action place\n");
		switch (a->data.place.path.type) {
		case PATH_DOT: printf("path_dot\n"); break;
		case PATH_HORZ: printf("path_horz\n");break;
		case PATH_VERT: printf("path_vert\n"); break;
		default: break;
		}
		printf("score: %d\n", a->data.place.score);
		break;
	}
	default: printf("action default?\n"); break;
	}
}

void printScore(const game_t *g)
{
	int i;
	
	NOT(g);
	
	for (i = 0; i < g->playerNum; i++) {
		printf("PLAYER_%d: %d\n", i, g->player[i].score);
	}
}

void printBoard(const board_t *b)
{
	for (int y = 0; y < BOARD_Y; y++) {
		for (int x = 0; x < BOARD_X; x++) {
			const tile_t *t = &b->tile[y][x];
			sq_t sq = b->sq[y][x];
			if (t->type != TILE_NONE) {
				printTile(t);
			} else {
                                char c = ' ';
				switch (sq) {
				case SQ_DBL_LET: c = '-'; break;
				case SQ_TRP_LET: c = '='; break;
				case SQ_DBL_WRD: c = '+'; break;
				case SQ_TRP_WRD: c = '#'; break;
				case SQ_FREE: c = '$'; break;
				case SQ_NORMAL: /* fall through */
				default: c = '.'; break;
				}
				putchar(c);
			}
		}
		printf("\n");
	}
}

void printRack(const player_t *p)
{
	for (int i = 0; i < RACK_SIZE; i++) {
		const tile_t *t = &p->tile[i];
		if (t->type != TILE_NONE) {
                        char c;
			switch(t->type) {
			case TILE_WILD:
                                c = '*';
                                break;
			case TILE_LETTER:
                                c = 'A' + t->letter;
                                break;
			default:
                                c = ' ';
                                break;
			}
			printf("(%d:%c)", i, c);
		}
	}
}

void printPlace(const move_place_t *mp)
{
	for (int i = 0; i < mp->num; i++) {
		printf("(%d,%d,%d) ", mp->coor[i].x, mp->coor[i].y, mp->rackIdx[i]);
	}
	printf("\n");
}

void printActionErr(const action_err_t err)
{
	switch (err) {
	case ACTION_ERR_UNKNOWN: puts("[err: unknown]"); break;
	case ACTION_ERR_PLACE_OUT_OF_RANGE: puts("[err: out of range]"); break;
	case ACTION_ERR_PLACE_SELF_OVERLAP: puts("[err: self overlap]"); break;
	case ACTION_ERR_PLACE_BOARD_OVERLAP: puts("[err: board overlap]"); break;
	case ACTION_ERR_PLACE_INVALID_RACK_ID: puts("[err: invalid rack id]"); break;
	case ACTION_ERR_PLACE_INVALID_SQ: puts("[err: place on free sq. or adjacent to a tile]"); break;
	case ACTION_ERR_PLACE_NO_RACK: puts("[err: no tiles placed on the board]"); break;
	case ACTION_ERR_PLACE_NO_DIR: puts("[err: tiles don't form a continuous line]"); break;
	case ACTION_ERR_PLACE_INVALID_PATH: puts("[err: invalid path]"); break;
	case ACTION_ERR_PLACE_INVALID_WORD: puts("[err: misspelled word(s)]"); break;
	case ACTION_ERR_DISCARD_RULE: puts("[err: discard rule]"); break;
	case ACTION_ERR_SKIP_RULE: puts("[err: skip rule]"); break;
	case ACTION_ERR_QUIT_RULE: puts("[err: quit rule]"); break;
	case ACTION_ERR_NONE: /* fall through */
	default: break;
	}
}

void printLog(const log_t *l)
{
	NOT(l);

	switch (l->type) {
	case ACTION_INVALID: {
		switch (l->data.err) {
		case ACTION_ERR_NONE: break;
		default: break;
		}
		break;
	}
	case ACTION_PLACE: {
		break;
	}
	case ACTION_DISCARD: {
		break;
	}
	case ACTION_SKIP: {
		break;
	}
	case ACTION_QUIT: {
		break;
	}
	default:
                break;
	}
}
