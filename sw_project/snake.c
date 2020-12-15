#include "snake.h"
#include "LCD_ILI9325.h"
#include "lcd_painting.h"
#include <stdlib.h>

void drawSegment(Segment* s) {
	drawRect(s->x, s->y, s->x+GRID_SIZE, s->y+GRID_SIZE, LCDBlack);
}

Segment* createSegment(){
    Segment* newSegment = malloc(sizeof(Segment));
    newSegment->next = NULL;
    return newSegment;
}

void initSnake(Snake* snake){
    snake->isAlive = 1;
    snake->head = createSegment();
    snake->head->x = 200;
    snake->head->y = 200;
}

void drawSnake(Snake* snake){
    Segment* temp = snake->head;
    while(temp != NULL){
        drawSegment(temp);
        temp = temp->next;
    }
}