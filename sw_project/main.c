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
volatile uint32_t tone = 400;
uint16_t max_score = 0;
char showing_end_screen = 0;
char restart = 0;

uint16_t samples[] = {1023,1022,1016,1006,993,976,954,931,904,874,841,806,768,728,
687,645,601,557,512,468,423,379,337,296,256,219,183,150,120,
93,69,48,31,18,8,2,0,2,8,18,31,48,69,93,120,150,183,219,256,
296,337,379,423,468,512,557,601,645,687,728,768,806,841,
874,904,931,954,976,993,1006,1016,1022,1023};

enum Direction direction = UP;

void takeTSCoords(int* x, int* y);

void setTone(short frequency) {
	uint16_t samplesSize = sizeof(samples) / sizeof(*samples);
	uint16_t divider = samplesSize * frequency;
	
	LPC_TIM1->MR0  = (uint16_t)(SystemCoreClock / 4 / divider);
}

void SysTick_Handler(void) {  /* SysTick interrupt Handler. */ 
	msTicks++; 
	directionTicks++;
	toneTicks++;
}

void DMA_IRQHandler() {
	LPC_GPDMACH0->DMACCSrcAddr = (uint32_t)samples;
	
	LPC_GPDMACH0->DMACCControl = 72 | (0b1 << 26) | (0b1 << 31) | (1 << 18) | (1 << 21);
	LPC_GPDMACH0->DMACCConfig = 1 | (0b1<<11) | (10 << 6) | (1 << 14) | (1 << 15); 
	
	LPC_GPDMA->DMACIntTCClear = 1;
	LPC_GPDMA->DMACIntErrClr = 1;
}

const uint16_t len = 600;
const uint16_t D = 587;
const uint16_t B = 493;
const uint16_t C = 523;
const uint16_t A = 440;
const uint16_t G = 392;
char flag = 0;

void TIMER1_IRQHandler(void) {
	if (toneTicks == len) {
			setTone(D);
	}	
	else if (toneTicks == 2 * len) {
		setTone(B);
	}	
	else if (toneTicks == 4 * len) {
		setTone(C);
	}
	else if (toneTicks == 5 * len) {
		setTone(A);
	}
	else if (toneTicks == 7 * len) {
		setTone(G);
	}
	else if (toneTicks == 7.5 * len) {
		setTone(B);
	}
	else if (toneTicks == 8 * len) {
		if (flag) {
			setTone(G);
			flag = 0;
		}
		else {
			setTone(D);
			flag = 1;
		}
		toneTicks = 0;
	}
	LPC_TIM1->IR = 1;
}

void drawCalibrationGrid() {
	char buffer[32];
	sprintf(buffer, "Click point number %d", currentCalibrationCorner+1);
	drawSentence(50, 50, buffer, 0);
	
	drawLetter(0, '1', 5, 5);	
	drawLetter(0, '2', 5, 300);
	drawLetter(0, '3', 225, 300);
	drawLetter(0, '4', 225, 5);
}

void EINT3_IRQHandler() {
	if (directionTicks < 200) {
		LPC_GPIOINT->IO0IntClr = PIN_TP_INT;
		return;
	}
	
	if (currentCalibrationCorner < 4) {
		int i = currentCalibrationCorner;
		takeTSCoords(&calibration_cords[i][0], &calibration_cords[i][1]);
		currentCalibrationCorner++;
		drawCalibrationGrid();
	}
	
	if (showing_end_screen) {
		restart = 1;
	}
	
	directionTicks = 0;
	NVIC_DisableIRQ(EINT3_IRQn);
	int x = 0;
	int y = 0;
	takeTSCoords(&x, &y);
	
	int middleY = 0;
	for (int i = 0; i < 4; ++i) {
		middleY += calibration_cords[i][1];
	}
	middleY /= 4;
	
	if (y < middleY) {direction = changeDirection(direction, TURN_LEFT);}
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
		samples[i] /= 2;
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
	LPC_TIM1->MCR = 3;
	LPC_TIM1->TCR = 1;
	setTone(D);
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
	LPC_GPDMACH0->DMACCControl = 72 | (0b1 << 26) | (0b1 << 31) | (1 << 18) | (1 << 21);
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
	
	clearScreen(LCDBlueSea);
	drawSentence(50, 50, "Click to start.", 0);
	showing_end_screen = 1;
	while(!restart);
	restart = 0;
	showing_end_screen = 0;
	
	char score_buffer[20];
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
		drawLine(LENGHT_LCD / 2, 0, LENGHT_LCD / 2, WIDTH_LCD, LCDRed);
		while (s.isAlive) {
			if (s.lenght == 26) {
				break;
			}
			
			while (msTicks < 200 - s.lenght * 5);
			msTicks = 0;
			drawFood(&food);
			moveSnakeHead(&s, direction);
			drawSnake(&s);
			putFoodOnGrid(&food, &s);
			
			checkCollision(&s, &food);
			drawLine(LENGHT_LCD / 2, 0, LENGHT_LCD / 2, WIDTH_LCD, LCDRed);
			sprintf(score_buffer, "Score: %d", (s.lenght - 1)* 100);
			drawSentence(80, 10, score_buffer, 0);
		}	
		
		if ((s.lenght - 1) * 100 > max_score) {max_score = (s.lenght - 1) * 100;}
		clearScreen(LCDBlueSea);
		if (s.isAlive) {
			drawSentence(50, 50, "Congratulations, you won.", 0);
			drawSentence(50, 70, score_buffer, 0);
			msTicks = 0;
			while (msTicks < 5000) {}
			msTicks = 0;
		}
		else {
			char buffer[20];
			sprintf(buffer, "Max score: %d", max_score);
			
			drawSentence(50, 50, "You lost.", 0);
			drawSentence(50, 70, score_buffer, 0);
			drawSentence(50, 90, buffer, 0);
			drawSentence(50, 150, "Click to restart.", 0);
			showing_end_screen = 1;
			while(!restart);
			showing_end_screen = 0;
			restart = 0;
		}
		deleteSnake(&s);
		direction = UP;
	}
	return 0;
}
