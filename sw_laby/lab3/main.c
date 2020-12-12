#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "Board_LED.h"                  // ::Board Support:LED

void send_string(const char* s) {
	int i = 0;
	char c;
	do {
		if (LPC_UART0->LSR & (0b1 << 5)) {
			c = s[i++];
			LPC_UART0->THR = c;
			}
		}	while (c != '\0');
	
}

void receive_and_resend() {
	char c;
	while((LPC_UART0->LSR & 0b1) == 0) {}
	c = LPC_UART0->RBR;
	LPC_UART0->THR = c;
}

void TIMER0_IRQHandler() {
	send_string("Ping ");
	LPC_TIM0->IR = 1;
}

void RTC_IRQHandler() {
	static int i = 0;
	if (i++ == 0) {
		send_string("Tik ");
	}
	else {
		send_string("Tak ");
		i = 0;
	}		
	LPC_RTC->ILR = 0b1;
}

void EINT0_IRQHandler() {
	send_string("Click ");
	static int led_state = 0;
	if (led_state++ == 0) {
		LED_On(3);
	}
	else {
		LED_Off(3);
		led_state = 0;
	}
	LPC_SC->EXTINT = 0b1;
}

void EINT3_IRQHandler() {
	send_string("Bang ");
	LPC_GPIOINT->IO0IntClr = 0b1 << 19;
}

int main() {
	/** inicjalizacja diod do zad 2c **/
	LED_Initialize();	
	LED_Off(2);
	/** **/
	
	/** konfiguracja uarta **/
	LPC_UART0->LCR = (1<<7)|3;
	LPC_UART0->DLL = 13;
	LPC_UART0->DLM = 0;
	LPC_UART0->FDR = 0xF1; //1/15
	LPC_UART0->LCR = 3;
		
	PIN_Configure(0, 2, 1, 0, 0);	// TXD0
	PIN_Configure(0, 3, 1, 0, 0);	// RXD0
	/** **/
	
	
	/** zad 2a - timer z ping co pol sek. **/
	LPC_TIM0->PR = 0;
	LPC_TIM0->MR0 = 12500000;
	LPC_TIM0->MCR = 3;
	LPC_TIM0->TCR = 1;
	NVIC_EnableIRQ(TIMER0_IRQn);
	/** **/ 
	
	
	/** zad 2b - rtc wysylajace tik i tak na przemian **/
	LPC_RTC->CCR = 0b1;
	LPC_RTC->ILR = 0b1;
	LPC_RTC->CIIR = 0b1;
	NVIC_EnableIRQ(RTC_IRQn);
	/** **/
	
	/** 2c miganie dioda **/
	PIN_Configure(2, 10, 1, 0, 0); // key2 tryb eint0, czwarty argument to mode, musi byc zerem bo pullup a eint jest na zboczu opadajacym
	LPC_SC->EXTMODE = 0b1;
	LPC_SC->EXTINT = 0;
	NVIC_EnableIRQ(EINT0_IRQn);
	/** **/
	
	
	/** ustawiamy 19 pin gpio na tristate, wlaczamy interrupt na falling edge, interrupty z gpio odpalaja eint3 **/
	PIN_Configure(0, 19, 0, 2, 0);
	LPC_GPIOINT->IO0IntEnF = 0b1 << 19;
	NVIC_EnableIRQ(EINT3_IRQn);
	/** **/
	
	
	
  	
	while(1) {
		
	}
	return 0;
}
