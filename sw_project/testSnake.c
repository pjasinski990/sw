#include "snake.h"
#include "stdio.h"
#include <stdlib.h> 
#include <time.h> 

void showSnake(Snake* snake){
    Segment* temp = snake->head;
    while(temp != NULL){
        printf("%d\t%d\n", temp->x, temp->y);
        temp = temp->next;
    }
}

void showFood(Food* food){
    
        printf("FOOD -> %d\t%d\t%d\n", food->x, food->y, food->isEaten);

}


enum Direction choosedir(enum Direction direction){
    int num = rand()%2;
    if(direction == RIGHT){
        if(num%2)
            return UP;
        return DOWN;
    }
    if(direction == LEFT){
        if(num%2)
            return DOWN;
        return UP;
    }
    if(direction == UP){
        if(num%2)
            return RIGHT;
        return LEFT;
    }
    if(direction == DOWN){
        if(num%2)
            return LEFT;
        return RIGHT;
    }
}
int main(){
    //srand(time(0));
    Snake snake;
    initSnake(&snake);
    Food food;
    initFood(&food);
    enum Direction direction = RIGHT;
    showSnake(&snake);
    showFood(&food);
    printf("-------------\n");
    for(int i = 0; i <5; i++){
        moveSnakeHead(&snake, LEFT);
        moveSnakeHead(&snake, UP);
        checkCollision(&snake,&food);
        if(snake.isAlive == 0){
            printf("DEAD\n");
            break;
        }
        putFoodOnGrid(&food, &snake);
    //         printf("Kolizja w iteracji %d\n", i);
    //         break;
    }
        moveSnakeHead(&snake, LEFT);
        moveSnakeHead(&snake, LEFT);
        moveSnakeHead(&snake, LEFT);
        moveSnakeHead(&snake, DOWN);
        moveSnakeHead(&snake, DOWN);

        checkCollision(&snake,&food);
        //putFoodOnGrid(&food, &snake);
        // moveSnakeHead(&snake, RIGHT);
        // moveSnakeHead(&snake, RIGHT);
    // for(int i = 0; i <12; i++){
        
    //     moveSnakeHead(&snake, UP);
    //     //extendSnake(&snake);
    //     if(!checkCollision(&snake)){
    //         printf("Kolizja w iteracji %d\n", i);
    //         break;
    //     }
    //     //showSnake(&snake);
    //     printf("-------------\n");
    // }
    // printf("-------------\n");
    // showSnake(&snake);
   // moveSnake(&snake);
   
    showSnake(&snake);
    showFood(&food);
    printf("Works!!!\n");
    deleteSnake(&snake);
    return 0;
}