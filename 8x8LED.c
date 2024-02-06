#include "TM4C123.h" 
#include <stdlib.h>
#include <stdint.h> 
#include "SPI_Init.h"
#include "I2C_Init.h"
#include "SysTick_Handler.h"
#include "math.h"
int volatile * const ADDRESS_X_MAG = (int *) 0x20001000; //our addresses for the LCD screen
int volatile * const ADDRESS_X_SIGN = (int *) 0x20001008;

int volatile * const ADDRESS_Y_MAG = (int *) 0x20001010;
int volatile * const ADDRESS_Y_SIGN = (int *) 0x20001018;

int volatile * const ADDRESS_Z_MAG = (int *) 0x20001020;
int volatile * const ADDRESS_Z_SIGN = (int *) 0x20001028;

extern void MAINS(void);
extern void DELAY100(void);
extern void CONFIG(void);

unsigned int First=0x900; //Our data to send in config for the 8x8
unsigned int Second=0xA00;
unsigned int Third=0xBF7;
unsigned int Fourth=0xFF0;
unsigned int Fifth=0xCF1;

int AngleIntX,AngleIntY,AngleIntZ; //declaration of variables
short int x,y,z,proof;
float AngleX,AngleY,AngleZ;
char data[100] = {0x08,0x80,0x15};
char data2[100];
float x_g,y_g,z_g;
int column,row,maxCR;
	
	 
void SysTick_Handler(void)
{		
I2C3_Read(0x53,0x32,6,&data2[1]);//Reading from the sensor 
		
z=(data2[6]<<8)+data2[5]+21;//21 is our offset
y=(data2[4]<<8)+data2[3];
x=(data2[2]<<8)+data2[1]+14;
	
z_g=(float)(z)*0.039; //our g to send angle function
y_g=(float)(y)*0.039;
x_g=(float)(x)*0.039;
	
AngleX=Angle_x(x_g,y_g,z_g); //Our angle calculations done in I2C_Init.h 
AngleY=Angle_y(x_g,y_g,z_g);
AngleZ=Angle_z(x_g,y_g,z_g);
		
		
		
if(AngleX<0){ //LCD logic starts here , also sends data to on board RGB LED
	   GPIOF->DATA=0x2;
		 *ADDRESS_X_SIGN=1;
			AngleX=-AngleX;
			
		}
else {
	  GPIOF->DATA&=~0x2;
		//We made some changes to fit into our orientation for the demo
		*ADDRESS_X_SIGN=0;
			
		}
if(AngleY<0){
	GPIOF->DATA=0x8;
	  
		*ADDRESS_Y_SIGN=1;
			AngleY=-AngleY;
		}
else {
		GPIOF->DATA&=~0x8;
		*ADDRESS_Y_SIGN=0;
			
		}
if(AngleZ<0){
		GPIOF->DATA=0x4;
		*ADDRESS_Z_SIGN=1;
			AngleZ=-AngleZ;
		}
else {
		GPIOF->DATA&=~0x4;
		*ADDRESS_Z_SIGN=0;
		
		}
AngleIntX=floor(AngleX); //Sending LCD magnitudes as integers
AngleIntY=floor(AngleY);
AngleIntZ=floor(AngleZ);
*ADDRESS_X_MAG = AngleIntX; //LCD magnitude reading
*ADDRESS_Y_MAG = AngleIntY;
*ADDRESS_Z_MAG = AngleIntZ;	
		
if(AngleY<30){//SPI LOGIC, our column and row data
		 if(*ADDRESS_Y_SIGN)column=4;
		 else column=5;
		 goto EndY;
		  
		}
if(AngleY<50){
		 if(*ADDRESS_Y_SIGN)column=3;
		 else column=6;
		 goto EndY;
			
		}
if(AngleY<70){
		 if(*ADDRESS_Y_SIGN)column=2;
		 else column=7;
		 goto EndY;
			
		}
if(*ADDRESS_Y_SIGN)column=1;
		else column=8;		
		EndY:	
if(AngleX<30){ //8x8 spi Logic to send
		 if(*ADDRESS_X_SIGN)row=3;
		 else row=4;
		 goto EndX;
		  
		}
if(AngleX<50){
		 if(*ADDRESS_X_SIGN)row=2;
		 else row=5;
		 goto EndX;
			
		}
if(AngleX<70){
		 if(*ADDRESS_X_SIGN)row=1;
		 else row=6;
		 goto EndX;
			
		}
if(*ADDRESS_X_SIGN)row=0;
   else row=7;	
	  EndX:

SPI_Transmit(0x100); //Clearing the screen
SPI_Transmit(0x200);
SPI_Transmit(0x300);
SPI_Transmit(0x400);
SPI_Transmit(0x500);
SPI_Transmit(0x600);
SPI_Transmit(0x700);
SPI_Transmit(0x800);
if(AngleY<10&&AngleX<10)goto END;
SPI_Transmit(0x000+(column<<8)+(1<<row)); //Sending SPI data with column and row data
		END:
SPI_Transmit(0xA00);
maxCR=(AngleIntX/10)+(AngleIntY/10);//Our increase in luminosity with higher accelaration
if(maxCR>15)maxCR=15;
SPI_Transmit(0xA00+maxCR);

}
 

int main()  
{
I2C3_Init();
SPI_Init();
SPI_Transmit(First);
SPI_Transmit(Second);
SPI_Transmit(Third);
SPI_Transmit(Fourth);
SPI_Transmit(Fifth);
I2C3_Write(0x53,0x2D,1,&data[0]); 
I2C3_Write(0x53,0x38,1,&data[1]);
SysTick_Init();
//All of our Initilizations and configrations done here
	
MAINS(); //Our main loop
}
