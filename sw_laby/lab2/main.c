#include "Board_LED.h"                  // ::Board Support:LED
#include "LPC17xx.h"
#include "Board_Joystick.h"             // ::Board Support:Joystick
#include "Board_Buttons.h"              // ::Board Support:Buttons

volatile uint32_t msTicks = 0;                              /* Variable to store millisecond ticks */  

void SysTick_Handler(void)  {                               /* SysTick interrupt Handler. */  
	msTicks++; 
	if(msTicks == 300){
		LED_On(0);
	}
	if(msTicks == 600){
		LED_Off(0);
		msTicks = 0;
	}
	/* See startup file startup_LPC17xx.s for SysTick vector */ 
}

void delay(uint32_t time){
	msTicks = 0;
	while(msTicks < time) {
		__WFI();
	}
}
int main(){
	 LED_Initialize();
	// LED_SetOut(0);
	// Buttons_Initialize();
	/*Determinuje co jaki czas wywoluje sie SysTick_Handler
	100M / 1000 = 0,001s*/
	SysTick_Config(SystemCoreClock / 1000);
	 
	LPC_GPIO0->FIODIR = 0xF;
	LPC_GPIO2->FIODIR = 0xFF;
	
	LPC_GPIO0->FIOCLR = 0xF;
	LPC_GPIO2->FIOCLR = 0xFF;
	LED_SetOut(0);

	/*
	uint32_t state;
	while(1) {
	state = Buttons_GetState();
	if (state == 3) {
		LPC_GPIO0->FIOSET = 0xF;
	} else {
		LPC_GPIO0->FIOCLR = 0xF;
	}
}
*/
/*
	for(uint32_t i = 0; i < 4; i++){
		LED_SetOut(0);
		LED_On(i);
		delay(300);
	}
*/
	//SCB->SCR|=(1<<1);
	while(1){
	//SCB->SCR|=(1<<1);
		__WFI();
		int i = 4;
	}
	return 0;
}
