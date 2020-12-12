#include "Board_LED.h"                  // ::Board Support:LED
#include "LPC17xx.h"

volatile uint32_t msTicks = 0;                              /* Variable to store millisecond ticks */  
static char diody[8] = {0x0A, 0xFF, 0x01, 1, 0, 1, 0xFF, 0x0};
static int index = 0;

void set_diodes(char* arr) {
	LPC_GPIO2->FIOPIN = ~diody[index++];
	if (index > 7) {index = 0;}
}

void DMA_IRQnHandler() {
	LPC_GPDMACH0->DMACCSrcAddr = (int)diody;
	LPC_GPDMACH0->DMACCDestAddr = (int)&LPC_GPIO2->FIOPIN;
	LPC_GPDMACH0->DMACCLLI = 0;
	LPC_GPDMACH0->DMACCControl = 8 | (0b1 << 26) | (0b1 << 31);
	LPC_GPDMACH0->DMACCConfig = 1 | (0b1<<12) | (0b1010 << 1);
	LPC_TIM1->IR = 1;
}


void delay(uint32_t time){
	msTicks = 0;
	while(msTicks < time) {
		__WFI();
	}
}
int main(){
	SysTick_Config(SystemCoreClock / 1000);
	LPC_GPIO2->FIODIR = 0xFF;	// sets selected pins to output
	LPC_GPIO2->FIOPIN = 0x0F;
	
	// timer 
	LPC_TIM1->PR = 1;
	LPC_TIM1->MR0 = 12500000;
	LPC_TIM1->MCR = 3;
	LPC_TIM1->TCR = 1;
	// NVIC_EnableIRQ(TIMER1_IRQn); // interrupt timera, niepotrzebny
	// */
	
	LPC_SC->PCONP |= 0b1 << 29;
	LPC_GPDMA->DMACConfig = 0b1;
	LPC_SC->DMAREQSEL = 0b1 << 2;
	LPC_GPDMA->DMACIntTCClear = 1;
	LPC_GPDMA->DMACIntErrClr = 1;
	
	LPC_GPDMACH0->DMACCSrcAddr = (int)diody;
	LPC_GPDMACH0->DMACCDestAddr = (int)&LPC_GPIO2->FIOPIN;
	LPC_GPDMACH0->DMACCLLI = 0;
	
	LPC_GPDMACH0->DMACCControl = 8 | (0b1 << 26) | (0b1 << 31);	// cel nie podlega inkrementacji
	// LPC_GPDMACH0->DMACCControl = 0b1 << 4; // osmiobitowe rozmiary
	// LPC_GPDMACH0->DMACCControl |= 0b1 << 31; // wlaczamy przerwanie
	
	LPC_GPDMACH0->DMACCConfig = 1 | (0b1<<12) | (0b1010 << 1);	// enable channel 0
	// LPC_GPDMACH0->DMACCConfig |= 1 << 12;	// typ peryferium pamiec
	// LPC_GPDMACH0->DMACCConfig |= 0b1010 << 1; // zrodlo timer1 prog 0
	// zerowanie bitu 14 clearuje IE 
	NVIC_EnableIRQ(DMA_IRQn);
	
	while(1) {}
	return 0;
}
