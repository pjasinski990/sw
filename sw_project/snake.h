/*Struct represents snake and functions connected with it*/

#ifndef __snake_H
#define __snake_H

#define TURN_LEFT 0
#define TURN_RIGHT 1

#define LENGHT_LCD 240 // do x
#define WIDTH_LCD 320 // do y

typedef struct Segment {
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

typedef struct {
	int isEaten;
	int x, y;
} Food;

/*Snake*/
void initSnake(Snake* snake);
void drawSnake(Snake* snake);

/*Zwolnienie pamieci w segmentach weza po jego smierci xD*/
void deleteSnake(Snake* snake);

/*Glowna funkcja odpowiadajaca za ruch calego weza*/
void moveSnakeHead(Snake* snake, enum Direction direction);

/*
Funkcja obslugujaca zderzenie weza z samym soba, z krawedzia oraz 
z jedzeniem
*/
int checkCollision(Snake* snake, Food* food);

/*Food*/
void initFood(Food* food);
void putFoodOnGrid(Food* food ,Snake* snake);
void drawFood(Food* food);


/*Private Snake*/
Segment* createSegment();
void drawSegment(Segment* s);
void extendSnake(Snake* snake);
void checkCollisionWithFood(Snake* snake, Food* food);
int checkCollisionWithItself(Snake* snake);
int checkCollisionWithBorders(Snake* snake);
void moveSnake(Snake* snake);

/*Private Food*/
int checkXFoodAndSnake(int x, Snake* snake);
int checkYFoodAndSnake(int y, Snake* snake);
/*
Funkcje private moga byc tylko w pliku .c, ale napisalem je tez tu, 
by wiedziec, ze istnieja
*/

enum Direction changeDirection(enum Direction actual_direction, int turn );
#endif