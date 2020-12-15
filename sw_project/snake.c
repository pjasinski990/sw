#include "snake.h"
#include "LCD_ILI9325.h"
#include "lcd_painting.h"
#include <stdlib.h>

/*
void drawSegment(Segment* s) {
	drawRect(s->x, s->y, s->x+GRID_SIZE, s->y+GRID_SIZE, LCDBlack);
}
*/
Segment* createSegment(){
    Segment* newSegment = malloc(sizeof(Segment));
    newSegment->next = NULL;
    return newSegment;
}


void initSnake(Snake* snake){
    snake->isAlive = 1;
    snake->lenght = 1;
    snake->head = createSegment();
    snake->head->x = 200;
    snake->head->y = 200;
}

/*
void drawSnake(Snake* snake){
    Segment* temp = snake->head;
    while(temp != NULL){
        drawSegment(temp);
        temp = temp->next;
    }
}
*/

void deleteSnake(Snake* snake){
    Segment* head = snake->head;
    Segment* next;
    while(head != NULL){
        next = head->next;
        free(head);
        head = next;
    }
}

/*
Bedzie jakis ustalony kierunek ruchu, w ktorym poruszal bedzie sie waz
i w zaleznosci czy klikniemy w lewo czy prawo bedzie zmieniac jego kierunek
ruchu
direction bedzie wybierala funckja, ktora bedzie sczytywala klikniete miejsce z ekranu
Wydaje mi sie, ze powinno to dzialac xDD
*/
void moveSnakeHead(Snake* snake, enum Direction direction){
    moveSnake(snake);
    switch (direction)
    {
    case LEFT:
        snake->head->x -= GRID_SIZE;
        break;
    
    case UP:
        snake->head->y -= GRID_SIZE;
        break;
    
    case RIGHT:
        snake->head->x += GRID_SIZE;
        break;
    
    case DOWN:
        snake->head->y += GRID_SIZE;
        break;
    default:
        snake->head->x -= GRID_SIZE;
        break;
    }
}

/*
Co do tego nie jestem przekoanny, ale powinno dzialac xD
moveSnake -> moveSnakeHead -> drawSnake
Dziala na zasadzie, ze z aktualnego segmentu przenosimy x i y do nastepnego segmentu
*/
void moveSnake(Snake* snake){
    if(snake->head->next == NULL) return;
    int x,y;
    int tempX, tempY;
    Segment* head = snake->head;
    Segment* next = snake->head->next;
    x = head->x;
    y = head->y;
    tempX = next->x;
    tempY = next->y;
    while(head->next != NULL){
        if(next->next != NULL){
            next = head->next;
            tempX = next->x;
            tempY = next->y;
        }
        next->x = x;
        next->y = y;
        x = tempX;
        y = tempY;
        head = head->next;
    }
}

/*
Zwiekszanie dlugosci weza
*/
void extendSnake(Snake* snake){
    snake->lenght++;
    Segment* lastSegment = snake->head;
    while(lastSegment->next != NULL){
        lastSegment = lastSegment->next;
    }

    Segment* newSegment = createSegment();
    lastSegment->next = newSegment;
    newSegment->x = lastSegment->x;
    newSegment->y = lastSegment->y;
}

/*
Zostaje zwrocone 0, jesli nastapila kolizja
*/

int checkCollision(Snake* snake){
    if(snake->lenght < 5) return 1;
    int xHead = snake->head->x;
    int yHead = snake->head->y;
    Segment* actualSegmentToChecking = snake->head->next;
    while(actualSegmentToChecking != NULL){
        if((xHead == actualSegmentToChecking->x) && (yHead == actualSegmentToChecking->y)){
            return 0;
        }
        actualSegmentToChecking = actualSegmentToChecking->next;
    }
    return 1;
}