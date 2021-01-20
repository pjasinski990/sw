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
volatile uint32_t toneTicks = 0;
volatile unsigned int v = 0;
volatile char tone = 2;

short samples[] = {1023,1022,1016,1006,993,976,954,931,904,874,841,806,768,728,
687,645,601,557,512,468,423,379,337,296,256,219,183,150,120,
93,69,48,31,18,8,2,0,2,8,18,31,48,69,93,120,150,183,219,256,
296,337,379,423,468,512,557,601,645,687,728,768,806,841,
874,904,931,954,976,993,1006,1016,1022,1023};

enum Direction direction = UP;

void takeTSCoords(int* x, int* y);

void setTone(short frequency) {
	short samplesSize = sizeof(samples) / sizeof(*samples);
	short divider = samplesSize * frequency;
	
	LPC_TIM1->MR0  = (uint32_t)(SystemCoreClock / 4 / divider);
}

void SysTick_Handler(void) {  /* SysTick interrupt Handler. */ 
	msTicks++; 
	directionTicks++;
	toneTicks++;

}

void DMA_IRQHandler() {
	LPC_GPDMACH0->DMACCSrcAddr = (uint32_t)samples;
	
	LPC_GPDMACH0->DMACCControl = 16 | (0b1 << 26) | (0b1 << 31) | (1 << 18) | (1 << 21);
	LPC_GPDMACH0->DMACCConfig = 1 | (0b1<<11) | (10 << 6) | (1 << 14) | (1 << 15); 
	
	LPC_GPDMA->DMACIntTCClear = 1;
	LPC_GPDMA->DMACIntErrClr = 1;
}

void TIMER1_IRQHandler(void) {
		// LPC_DAC->DACR = samples[v];
		// v++;
    // v %= sizeof(samples) / sizeof(*samples);

		LPC_TIM1->IR = 1;
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


void prepareSamples(){
	for (int i = 0; i < sizeof(samples)/sizeof(*samples); ++i) {
		samples[i] <<= 6;
	}
}

int main() {
	prepareSamples();
	
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
	
	/* ------------------------------------------------------------ */
	/* DAC */
	
	    	
	PIN_Configure(0, 26, PIN_FUNC_2, PIN_PINMODE_TRISTATE, PIN_PINMODE_NORMAL);	// gpio dac 
	// LPC_DAC->DACCTRL = 1 << 2 | 1 << 3; // timer enabled, dma burst request enabled 
	// LPC_DAC->DACCNTVAL = 12500; // dac timer for dma requesting 
	 

	/* timer 1 dla DMA */
	LPC_TIM1->PR = 0;
	setTone(400);
	LPC_TIM1->MCR = 3;
	LPC_TIM1->TCR = 1;
	NVIC_EnableIRQ(TIMER1_IRQn);

	/* ----------------------- */ 

		
	/* ------------------------------------------------------------ */
	/* DMA */
	LPC_SC->PCONP |= 0b1 << 29;
	LPC_GPDMA->DMACConfig = 0b1;
	LPC_SC->DMAREQSEL = 0b1 << 2;	// select timer 1 match 0 as dma request
	LPC_GPDMA->DMACIntTCClear = 1;
	LPC_GPDMA->DMACIntErrClr = 1;
 
	LPC_GPDMACH0->DMACCSrcAddr = (uint32_t)samples; 
	LPC_GPDMACH0->DMACCDestAddr = (uint32_t)&LPC_DAC->DACR; 
	LPC_GPDMACH0->DMACCLLI = 0; 
	LPC_GPDMACH0->DMACCControl = 16 | (0b1 << 26) | (0b1 << 31) | (1 << 18) | (1 << 21); // TO MOZE BYC 2<<18 OGARNAC
	// 1 << 26 - zrodlo jest inkrementowane (cel nie)
	// 16 - szesnastobitowe rozmiary
	// 1 << 31 - wlaczamy przerwanie

	LPC_GPDMACH0->DMACCConfig = 1 | (0b1 << 11) | (10 << 6) | (1 << 14) | (1 << 15);      // enable channel 0
 	// 1 << 11 - typ pamiec peryferium
 	// 10 << 6 - zrodlo timer ( jezeli dobrze myslimy i bity 10:6 to te od peryferium docelowego)
 	// zerowanie bitu 14 clearuje IE
	NVIC_EnableIRQ(DMA_IRQn);

	/* ------------------------*/	

	drawCalibrationGrid();
	while (currentCalibrationCorner < 4);
	
	while(1) {
		//clearScreen(LCDBlueSea);
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
