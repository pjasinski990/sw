#ifndef __temp_H
#define __temp_H

#include "snake.h"

#define TURN_LEFT 0
#define TURN_RIGHT 1

/*
Funckja szczytujaca wartosci z TS zwraca czy to jest lewa 
czy prawa strona ekranu i przekazuje tę wartość jako turn
*/
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
#endif
