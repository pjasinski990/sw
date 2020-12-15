#include "snake.h"

void drawSegment(segment* s, int gridsize) {
	drawRect(s->x, s->y, s->x+gridsize, s->y+gridsize, LCDBlack);
}