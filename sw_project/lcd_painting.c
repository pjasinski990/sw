#include "lcd_painting.h"
//#include "LPC17xx.h"                    // Device header
#include "Open1768_LCD.h"
//#include "Board_LED.h"                  // ::Board Support:LED
#include "LCD_ILI9325.h"
#include "asciiLib.h"
#include "TP_Open1768.h"
#include <math.h>


void drawPoint(unsigned int x, unsigned int y, unsigned int color) {
	lcdSetCursor(x, y);
	lcdWriteIndex(DATA_RAM);
	lcdWriteData(color);
}

void drawLine(float x0, float y0, float x1, float y1, unsigned int color) {
	float deltay = y1 - y0;
	float deltax = x1 - x0;
	
	if (fabs(deltax) < 1e-8) {
		while (y0 < y1) {
			drawPoint(x0, y0, color);
			y0 += 1.0f;
		}
		return;
	}
	
	float deltaerr = fabs(deltay / deltax);
	float error = 0.0f;
	int y = y0;
	
	while (x0 < x1) {
		drawPoint(x0, y, color);
		error = error + deltaerr;
		
		if (error >= 0.5f) {
			y = y + sign(deltay);
			error -= 1.0f;
		}
		x0 += 1.0f;
	}
}

void drawEmptyRect(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color) {
	unsigned int deltax = x1 - x0;
	unsigned int deltay = y1 - y0;
	
	drawLine(x0, y0, x0 + deltax, y0, color);
	drawLine(x0, y1, x0 + deltax, y1, color);
	drawLine(x0, y0, x0, y0+deltay, color);
	drawLine(x0, y1, x0, y1+deltay, color); // cos nie tak
}

void drawRect(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color) {
	lcdSetCursor(x0, y0);
	lcdWriteReg(HADRPOS_RAM_START, x0);
	lcdWriteReg(HADRPOS_RAM_END, x1 - 1);
	lcdWriteReg(VADRPOS_RAM_START, y0);
	lcdWriteReg(VADRPOS_RAM_END, y1 - 1);
	lcdWriteIndex(DATA_RAM);
	int limit = (x1 - x0) * (y1 - y0);
	for (int i = 0; i < limit; i++) {
				lcdWriteData(color);
	}
}

/*Rysowanie z tlem*/
void drawLetter(unsigned int font, char letter, unsigned int xpos, unsigned int ypos) {
	unsigned char buffer[16];
	GetASCIICode(font, buffer, letter);
	lcdSetCursor(xpos, ypos);								//za kazdym razem ustawiamy kurson do rysowania litery
	lcdWriteReg(HADRPOS_RAM_START, xpos);		//poczatek ramki
	lcdWriteReg(HADRPOS_RAM_END, xpos + 7); // pozioma wartosc boxa na litere
																					// musi byc dlugosc - 1 
	lcdWriteReg(VADRPOS_RAM_START, ypos);
	lcdWriteReg(VADRPOS_RAM_END, ypos + 15); // pionowa wartosc boxa -koniec ramki 
	lcdWriteIndex(DATA_RAM);
	
	for (int i = 0; i < 16; i++) {
		for (int j = 7; j >= 0; j--) {
			if (buffer[i] & (0b1 << j)) { 			 //sprawdzanie wartosci poszczegolnego bitu, ktory mamy pomalowac - jesli != 0 -> czarny
				lcdWriteData(LCDBlack);
			}
			else {
				lcdWriteData(LCDWhite);
			}
		}
	}
}

/*Rysowanie bez tla*/
void drawLetter_v2(unsigned int font, char letter, unsigned int xpos, unsigned int ypos) {
	unsigned char buffer[16];
	GetASCIICode(font, buffer, letter);
	lcdWriteIndex(DATA_RAM);
	
	for (int i = 0; i < 16; i++) {
		for (int j = 7; j >= 0; j--) {
			if (buffer[i] & (0b1 << j)) { 			 //sprawdzanie wartosci poszczegolnego bitu, ktory mamy pomalowac - jesli != 0 -> czarny
				lcdSetCursor(xpos + j, ypos + i);
				lcdWriteData(LCDBlack);
			}
		}
	}
}

void drawSentence(unsigned int xpos, unsigned int ypos, const char* sentence, unsigned int font) {
	unsigned int actual_x = xpos;
	unsigned int actual_y = ypos;
	const unsigned int deltax = 8;					//szerokosc poszczegolnej litery
	const unsigned int deltay = 16;					//wysokosc poszczegolnej litery
	
	while(*sentence != 0) {
		if(actual_x + deltax > 240) {					//sprawdzanie czy nie wychodzimy poza ekran		
			actual_x = xpos;
			actual_y += deltay;
		}
		drawLetter(font, *sentence, actual_x, actual_y);
		actual_x += deltax;
		sentence++;
	}
}

void clearScreen(unsigned int color) {
	drawRect(0, 0, 241, 321, color);
}

void printCords(int x, int y) {
	char xbuffer[24];
	char ybuffer[24];
	sprintf(xbuffer, "%d", x);
	sprintf(ybuffer, "%d", y);
	
	drawSentence(0, 0, xbuffer, 0);
	drawSentence(0, 20, ybuffer, 0);
}

void printTSCords() {
	int x, y;
	touchpanelGetXY(&x, &y);
	printCords(x, y);
}

void drawCalibrationX(unsigned int x, unsigned int y) {
	drawLine(x-5, y, x+5, y, LCDBlack);
	drawLine(x, y-5, x, y+5, LCDBlack);
}

