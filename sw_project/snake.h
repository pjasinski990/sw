/*Struct represents snake and functions connected with it*/

#ifndef __snake_H
#define __snakeg_H

typedef struct {
	struct Segment* next;
	int x, y;
} Segment;

typedef struct {
	int isAlive;
	Segment* head;
	
} Snake;

void drawSegment(Segment* s, int gridsize);
#endif