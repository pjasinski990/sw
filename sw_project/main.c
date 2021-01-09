#include "LPC17xx.h"                    // Device header
#include "Open1768_LCD.h"
#include "Board_LED.h"                  // ::Board Support:LED
#include "LCD_ILI9325.h"
#include "asciiLib.h"
#include "TP_Open1768.h"
#include "PIN_LPC17xx.h"                // Keil::Device:PIN

#include <stdlib.h>
#include <math.h>
#include "lcd_painting.h"
#include "snake.h"

int currentCalibrationCorner = 0;
int calibration_cords[4][2] = {0};
volatile uint32_t msTicks = 0;
volatile uint32_t directionTicks = 0;

enum Direction direction = UP;

void takeTSCoords(int* x, int* y);

void SysTick_Handler(void) {  /* SysTick interrupt Handler. */  
	msTicks++; 
	directionTicks++;
}


void drawCalibrationGrid() {
	char buffer[32];
	sprintf(buffer, "Click point number %d", currentCalibrationCorner+1);
	drawSentence(50, 50, buffer, 0);
	
	drawLetter(0, '1', 20, 5);
	drawCalibrationX(10, 10);
	
	drawLetter(0, '2', 15, 310);
	drawCalibrationX(230, 310);
	
	drawLetter(0, '3', 225, 310);
	drawCalibrationX(10, 310);

	drawLetter(0, '4', 205, 5);
	drawCalibrationX(230, 10);
}

void EINT3_IRQHandler() {
	if (directionTicks < 300) {
		LPC_GPIOINT->IO0IntClr = PIN_TP_INT;
		return;
	}
	
	if (currentCalibrationCorner < 4) {
		int i = currentCalibrationCorner;
		takeTSCoords(&calibration_cords[i][0], &calibration_cords[i][1]);
		currentCalibrationCorner++;
		drawCalibrationGrid();
	}
	
	directionTicks = 0;
	NVIC_DisableIRQ(EINT3_IRQn);
	int x = 0;
	int y = 0;
	takeTSCoords(&x, &y);
	if (x < (calibration_cords[0][0] + calibration_cords[1][0]) / 2) {direction = changeDirection(direction, TURN_LEFT);}
		else {direction = changeDirection(direction, TURN_RIGHT);}
		
	NVIC_EnableIRQ(EINT3_IRQn);
	LPC_GPIOINT->IO0IntClr = PIN_TP_INT;
}


void takeTSCoords(int* resx, int* resy) {
	const int iterations = 15;
	int tempx, tempy;
	int xvals[iterations];
	int yvals[iterations];
	
	for (int i = 0; i < iterations; ++i) {
		touchpanelGetXY(&tempx, &tempy);
		xvals[i] = tempx;
		yvals[i] = tempy;
	}
	int sumx, sumy;
	sumx = sumy = 0;
	for (int i = 0; i < iterations; i++) {
		sumx += xvals[i];
		sumy += yvals[i];
	}
	*resx = sumx / iterations;
	*resy = sumy / iterations;
	msTicks = 0;
}

/*
	naprawic kalibracje
	
*/
int main() {
	lcdConfiguration();
	init_ILI9325();
	touchpanelInit();
	
	NVIC_SetPriority(EINT3_IRQn, 1);
	SysTick_Config(SystemCoreClock / 1000);
	
	// gpio interrupt on eint3
	PIN_Configure(0, 19, 0, 2, 0);
	LPC_GPIOINT->IO0IntEnF = 0b1 << 19;
	
	clearScreen(LCDBlueSea);
	NVIC_EnableIRQ(EINT3_IRQn);
	
	drawCalibrationGrid();
	while (currentCalibrationCorner < 4);
	
	while(1) {
		clearScreen(LCDBlueSea);
		int middleX = (calibration_cords[0][0] + calibration_cords[1][0]) / 2;
		//const int GRID_SIZE = 20;
		
		Snake s;
		initSnake(&s);
		Food food;
		initFood(&food);
		/*
		Aktualny kierunek ruchu weza, ktory nawet powienien byc globalny i 
		volatile, by przerwanie moglo go modyfikowac
		*/
		while (s.isAlive) {
			while (msTicks < 300);
			msTicks = 0;
			clearScreen(LCDBlueSea);
			drawFood(&food);
			drawSnake(&s);
			
			moveSnakeHead(&s, direction);
			putFoodOnGrid(&food, &s);
			checkCollision(&s, &food);
		}	
		deleteSnake(&s);
		direction = UP;
	}
	return 0;
}
