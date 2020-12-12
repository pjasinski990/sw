#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"                // Keil::Device:PIN


int main() {
	
	//PIN_Configure(0, 3, PIN_FUNC_1, PIN_PINMODE_TRISTATE, PIN_PINMODE_NORMAL);	// RXD0 
	//PIN_Configure(0, 2, PIN_FUNC_1, PIN_PINMODE_TRISTATE, PIN_PINMODE_NORMAL);	// TXD0
	
	LPC_UART0->LCR = (1<<7)|3;
	LPC_UART0->DLL = 13;
	LPC_UART0->DLM = 0;
	LPC_UART0->FDR = 0x1F; //1/15
	LPC_UART0->LCR = 3;
	 
	PIN_Configure(0, 2, 1, 0, 0);	// TXD0
	PIN_Configure(0, 3, 1, 0, 0);	// RXD0
	
	LPC_UART0->THR = 'A';
	LPC_UART0->THR = 'A';
	LPC_UART0->THR = 'A';
	LPC_UART0->THR = 'A';
	LPC_UART0->THR = 'A';
	
	while(1) {
	}
	return 0;
}
