/*Drawing functions*/
#ifndef __lcd_painting_H
#define __lcd_painting_H

void drawPoint(unsigned int x, unsigned int y, unsigned int color);
void drawLine(float x0, float y0, float x1, float y1, unsigned int color);
void drawEmptyRect(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color);
void drawRect(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color);

/*Rysowanie z tlem*/
void drawLetter(unsigned int font, char letter, unsigned int xpos, unsigned int ypos);

/*Rysowanie bez tla*/
void drawLetter_v2(unsigned int font, char letter, unsigned int xpos, unsigned int ypos);
void drawSentence(unsigned int xpos, unsigned int ypos, const char* sentence, unsigned int font);

void clearScreen(unsigned int color);
void printCords(int x, int y);
void printTSCords();
void drawCalibrationX(unsigned int x, unsigned int y);

float sign(float x) {
	return x < 0? -1.0f:1.0f;
}
#endif