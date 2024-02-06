#include "TM4C123.h" 
void SysTick_Init(){
SYSCTL->RCGCGPIO |= 0x20; //Wait for 
	while((SYSCTL->PRGPIO)&0x20==0){}
GPIOF->DIR |= 0x0E;
GPIOF->DEN |= 0x0E;
		
/* Systick Timer Interrupt Initilization */
SysTick->CTRL=0;
//SysTick->LOAD = 15999999; // one second delay relaod value
SysTick->LOAD = 1699999; 
SysTick->VAL  = 0; //initialize current value register 
SysTick->CTRL = 7 ; // enable counter, interrupt and select system bus clock 
	}

	