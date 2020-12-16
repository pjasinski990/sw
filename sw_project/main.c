#include "LPC17xx.h"                    // Device header
#include "Open1768_LCD.h"
#include "Board_LED.h"                  // ::Board Support:LED
#include "LCD_ILI9325.h"
#include "asciiLib.h"
#include "TP_Open1768.h"

#include <stdlib.h>
#include <math.h>
#include "lcd_painting.h"
#include "snake.h"

int calibration_cords[4][2] = {0};
volatile uint32_t msTicks = 0;

void SysTick_Handler(void)  {                               /* SysTick interrupt Handler. */  
	msTicks++; 
}



void takeTSCords(int* resx, int* resy) {
	const int iterations = 15;
	int tempx, tempy;
	int xvals[iterations];
	int yvals[iterations];
	
	while(msTicks < 2400) {}
	msTicks = 0;
	
	touchpanelGetXY(&tempx, &tempy);
	for (int i = 0; i < 15; i++) {
		while(tempx == 4095 || tempx == 0 || tempy == 4095 || tempy == 0) {
			touchpanelGetXY(&tempx, &tempy);
			i = 0;
		}
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
}


void calibrateTS() {
	drawLetter(0, '1', 20, 5);
	drawCalibrationX(10, 10);
	
	drawLetter(0, '2', 15, 310);
	drawCalibrationX(230, 310);
	
	drawLetter(0, '3', 225, 310);
	drawCalibrationX(10, 310);

	drawLetter(0, '4', 205, 5);
	drawCalibrationX(230, 10);

	for (int i = 0; i < 4; i++) {
		char buffer[32];
		sprintf(buffer, "Click point number %d", i+1);
		drawSentence(50, 50, buffer, 0);
		takeTSCords(&calibration_cords[i][0], &calibration_cords[i][1]);
	}
	
	clearScreen(LCDBlueSea);
	drawSentence(50, 50, "Calibration finished", 0);
}

void convertTStoLCD(int xts, int yts, int* xres, int* yres) {
	// TODO
	*xres = xts;
	*yres = yts;
}




/*
	funkcja decydujaca o kierunku
	funkcja skalujaca koordynaty z ts
	generowanie startowego weza + 
	kolizja z jedzeniem i sciana/innym segmentem + (trzeba jedzenie zrobic)
	przerwania z tsa 
	przesuwanie calego weza + 
	generowanie jedzenia na mapie
*/
int main() {
	lcdConfiguration();
	init_ILI9325();
	touchpanelInit();
	SysTick_Config(SystemCoreClock / 1000);
	
	clearScreen(LCDBlueSea);
	calibrateTS();
	
	while(1) {
		clearScreen(LCDBlueSea);
		//const int GRID_SIZE = 20;
		
		Snake s;
		initSnake(&s);
		Food food;
		initFood(&food);
		/*
		Aktualny kierunek ruchu weza, ktory nawet powienien byc globalny i 
		volatile, by przerwanie moglo go modyfikowac
		*/
		enum Direction direction = UP;
		while (s.isAlive) {
			while (msTicks < 300);
			msTicks = 0;
			clearScreen(LCDBlueSea);
			drawSnake(&s);
			//drawSegment(s.head);
			//moveSegments(s);
			
			//drawSnake();
			//checkCollision();
		}	
	}
	return 0;
}
