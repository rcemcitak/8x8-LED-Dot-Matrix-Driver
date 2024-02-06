#include "TM4C123.h" 
#include <stdlib.h>
#include <stdint.h> 


void SPI_Transmit(unsigned int data)
{
	

  while((SSI2->SR & 0x2) == 0x0); // wait until transmit FIFO is not full  
  	
	SSI2->DR = data; // send the data 
	
	while((SSI2->SR & 0x2) == 0x10); //  wait until busy bit becomes 0 
  

}



void SPI_Init(){


 	SYSCTL->RCGCGPIO |= 0x2; // Enable and provide a clock to GPIO PortB 
		
	while(((SYSCTL->PRGPIO)&0x2)==0){}

	GPIOB->DEN |= 0xF0; // Enable digital functions for PB4, PB5, PB6, PB7 
	GPIOB->AMSEL &= ~0xF0U; //Disable Analog function for PB4, PB5, PB6, PB7 
		
	GPIOB->AFSEL |= 0xF0; // Enable alternate functions on PB4, PB5, PB6, PB7 
	GPIOB->PCTL &=~ 0xFFFF0000U; // Assign SPI signals to PB4, PB5, PB6, PB7
	
	GPIOB->PCTL |= 0x22220000U; // Assign SPI signals to PB4, PB5, PB6, PB7 
	GPIOB->DIR |= 0xF0; // Set PB4, PB5, PB6, PB7 as output 	
	
	GPIOB->PUR |= 0x20; // Make slave select line low when idle 
		
	SYSCTL->RCGCSSI |= 0x4; // Enable and provide a clock to SPI2
	while(((SYSCTL->PRSSI)&0x4)==0){}
	
	SSI2->CR1 = 0x0; // Disable SPI and configure it as a master 
	SSI2->CC = 0x0; // Select the SPI Baud Clock Source as system clock
  
	SSI2->CPSR |= 0x2; // Set the clock frequency to 8 MHz 
		
  SSI2->CR0 = 0xF; // Freescale SPI mode, 1 Mbps bit rate, 16 data bits 	 	
		
  SSI2->CR1 |= 0x2; // Enable SPI  
	
	
}