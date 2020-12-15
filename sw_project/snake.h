/*Struct represents snake and functions connected with it*/

#ifndef __snake_H
#define __snake_H

#define GRID_SIZE 20
typedef struct {
	struct Segment* next;
	int x, y;
} Segment;

typedef struct {
	int isAlive;
	int lenght;
	Segment* head;
	
} Snake;

enum Direction{
    LEFT, UP, RIGHT, DOWN
};

void drawSegment(Segment* s);
Segment* createSegment();
void initSnake(Snake* snake);
void drawSnake(Snake* snake);
void deleteSnake(Snake* snake);
void moveSnakeHead(Snake* snake, enum Direction direction);
void moveSnake(Snake* snake);
void extendSnake(Snake* snake);
int checkCollision(Snake* snake);

#endif