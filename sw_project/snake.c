#include "snake.h"
#include "LCD_ILI9325.h"
#include "lcd_painting.h"
#include <stdlib.h>

#define GRID_SIZE 20

#define START_POSITION_X 200
#define START_POSITION_Y 200

/*Jesli wymiary beda na odwrot, wystarczy zamienic ponizsze wartosci*/
#define LENGHT_LCD 240 // do x
#define WIDTH_LCD 320 // do y

#define START_FOOD_POSITION_X 100
#define START_FOOD_POSITION_Y 100

/*
Elementy zakomentowane musialem zakomentowac xDD , by moc 
skompilowac to u siebie na kompie bez tych fancy bibliotek
*/

void drawSegment(Segment* s) {
	drawRect(s->x, s->y, s->x+GRID_SIZE, s->y+GRID_SIZE, LCDBlack);
}
int position_to_paint_over_X = -1;
int position_to_paint_over_Y = -1;
 

void drawSnake(Snake* snake){
    drawSegment(snake->head);
    /*Painting over last segment of the snake*/
    drawRect(position_to_paint_over_X, position_to_paint_over_Y, position_to_paint_over_X + GRID_SIZE, position_to_paint_over_Y + GRID_SIZE, LCDBlueSea);
    // Segment* temp = snake->head;
    // while(temp != NULL){
    //     drawSegment(temp);
    //     temp = temp->next;
    // }
}


void drawFood(Food* food){
    drawRect(food->x, food->y, food->x+GRID_SIZE, food->y+GRID_SIZE, LCDGreen);
}



Segment* createSegment(){
    Segment* newSegment = malloc(sizeof(Segment));
    newSegment->next = NULL;
    return newSegment;
}

void initSnake(Snake* snake){
    snake->isAlive = 1;
    snake->lenght = 1;
    snake->head = createSegment();
    snake->head->x = START_POSITION_X;
    snake->head->y = START_POSITION_Y;
}



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

moveSnakeHead -> drawSnake
Dziala na zasadzie, ze z aktualnego segmentu przenosimy x i y do nastepnego segmentu
*/
void moveSnake(Snake* snake){
    if(snake->head->next == NULL){
        position_to_paint_over_X = snake->head->x;
        position_to_paint_over_Y = snake->head->y;
        return;
    } 
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
    position_to_paint_over_X = x;
    position_to_paint_over_Y = y;
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
Zostaje zwrocone 0 i ustawione isAlive weza na 0 jesli nastapila kolizja
*/
int checkCollision(Snake* snake, Food* food){
    /*
    Sprawdzenie czy nie wychodzimy poza ekran, jesli tak, 
    to ustawiamy flage zycia weza na 0
    */
    if(!checkCollisionWithBorders(snake)){
        snake->isAlive = 0;
        return 0;
    } 
    /*Sprawdzenie czy waz natknal sie na jedzenie*/
    checkCollisionWithFood(snake, food);

    /*
    Jesli jest dlugosc weza mniejsza niz 3, to i tak nie moze miec kolizji samemu z soba
    UWAGA ->  Przy zmianie respawnu weza trzeba bedzie to zmienic, bo wtedy wychodzedznie
    poza tablice jest zagrozone
    Jest to po to, by w trakcie rozszerzania sie nie wystapila kolizja samego z soba
    */
    if(snake->lenght < 3) return 1;

    /*Sprawdzanie czy nastapila kolizja z samym soba*/
    if(!checkCollisionWithItself(snake)){
        snake->isAlive = 0;
        return 0;
    }

    return 1;
}

/*
Funkcja sprawdza kolizje weza z samym soba
*/
int checkCollisionWithItself(Snake* snake){
    int xHead = snake->head->x;
    int yHead = snake->head->y;
    
    Segment* actualSegmentToChecking = snake->head->next;
    while(actualSegmentToChecking != NULL){
        if((xHead == actualSegmentToChecking->x) && (yHead == actualSegmentToChecking->y)){
            snake->isAlive = 0;
            return 0;
        }
        actualSegmentToChecking = actualSegmentToChecking->next;
    }
    return 1;
}

/*
Sprawdzenie czy nie wychodzimy poza zakres ekranu
Co do LENGHT i WIDTH nie jestem pewien czy nie maja byc na odwrot
*/
int checkCollisionWithBorders(Snake* snake){

    if(snake->head->x < 0)
        return 0;
    if(snake->head->x >= LENGHT_LCD)
        return 0;
    if(snake->head->y < 0)
        return 0;
    if(snake->head->y >= WIDTH_LCD)
        return 0;
    return 1;
}

/*
Sprawdzenie czy waz natknal sie na jedzenie, jesli tak zwiekszamy weza i
flaga isEaten ustawiona jest na 1 i wtedy funkcja putFoodOnGrid znalezc 
nowe polozenie dla jedzenia
*/
void checkCollisionWithFood(Snake* snake, Food* food){

    if(food->isEaten == 1) return;

    if((snake->head->x == food->x) && (snake->head->y == food->y)){
        extendSnake(snake);
        food->isEaten = 1;
    }
}


/*FOOD*/
void initFood(Food* food){
    food->isEaten = 0;
    food->x = START_FOOD_POSITION_X;
    food->y = START_FOOD_POSITION_Y;
}

/*
Funkcja sprawdza, czy moze w wylosowanych miejscu wstawic jedzenie, 
jesli tak to wstawia to jedzie na wylosowane wspolrzedne i ustawia flage 
isEaten na 0
*/
void putFoodOnGrid(Food* food ,Snake* snake){
    /*Jesli jedzenie nie jest zjedzone, to nic sie nie dzieje*/
    if(food->isEaten == 0)
        return;
    int x, y;
    /*-2 i + 1 dlatego, by uniknac jedzenia przy krawedziach*/
    x = (rand()%(LENGHT_LCD / GRID_SIZE - 2) + 1 ) * GRID_SIZE;
    y = (rand()%(WIDTH_LCD / GRID_SIZE - 2) + 1 ) * GRID_SIZE;

    /*Wylosowanie odpowiednich x i y, ktore nie leza na wezu*/
    while(checkXFoodAndSnake(x, snake) && (checkYFoodAndSnake(y, snake))){
        x = (rand()%(LENGHT_LCD / GRID_SIZE - 2) + 1 ) * GRID_SIZE;
        y = (rand()%(WIDTH_LCD / GRID_SIZE - 2) + 1 ) * GRID_SIZE;
    }

    food->x = x;
    food->y = y;
    food->isEaten = 0;

}

/*Zwraca 1, kiedy x i jakikolwiek segment w snaku maja x taki sam*/
int checkXFoodAndSnake(int x, Snake* snake){
    Segment* temp = snake->head;
    int flag = 0;
    while(temp != NULL){
        if(x == temp->x){
            flag = 1;
            break;
        }
        temp = temp->next;
    }
    return flag;
}

int checkYFoodAndSnake(int y, Snake* snake){
    Segment* temp = snake->head;
    int flag = 0;
    while(temp != NULL){
        if(y == temp->y){
            flag = 1;
            break;
        }
        temp = temp->next;
    }
    return flag;
}

enum Direction changeDirection(enum Direction actual_direction, int turn ){
    if(actual_direction == UP){
        if(turn == TURN_LEFT){
            return LEFT;
        }
        else
        {
            return RIGHT;
        }
    }

    if(actual_direction == LEFT){
        if(turn == TURN_LEFT){
            return DOWN;
        }
        else
        {
            return UP;
        }
    }

    if(actual_direction == DOWN){
        if(turn == TURN_LEFT){
            return RIGHT;
        }
        else
        {
            return LEFT;
        }
    }

    if(actual_direction == RIGHT){
        if(turn == TURN_LEFT){
            return UP;
        }
        else
        {
            return DOWN;
        }
    }
}

