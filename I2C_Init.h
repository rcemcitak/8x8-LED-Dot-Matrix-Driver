#include "TM4C123GH6PM.h"
#include "math.h"

void I2C3_Init ( void );  
int Delay(int a);
char I2C3_Write(int slave_address, char slave_memory_address, int bytes_count, char* data);
char I2C3_Read(int slave_address, char slave_memory_address, int bytes_count, char* data);
float Angle_x(float x ,float y , float z);
float Angle_y(float x, float y , float z);
float Angle_z(float x, float y , float z);

float Angle_x(float x, float y , float z){ //Calculating angles with 360 degree 3D-Space
float tangent;
float sqrt1=(float) pow(y,2.0)+(float) pow(z,2.0);
sqrt1=(float)sqrt(sqrt1);
tangent=(float)atan(x/sqrt1);
tangent=(float)(180.0/3.14)*tangent;

return tangent;	
}
float Angle_y( float x, float y, float z){
float tangent;
float sqrt1= (float) pow(x,2)+(float) pow(z,2);
sqrt1=(float) sqrt(sqrt1);
tangent=(float) atan(y/sqrt1);
tangent=(float) (180.0/3.14)*tangent;

return tangent;
}
float Angle_z( float x, float y, float z){
float tangent;
float sqrt1= (float) pow(x,2)+ (float) pow(y,2);
sqrt1=(float) sqrt(sqrt1);
tangent=(float) atan(z/sqrt1);
tangent=(float) (180.0/3.14)*tangent;

return tangent;
}

// I2C intialization and GPIO alternate function configuration
void I2C3_Init ( void )
{
SYSCTL->RCGCGPIO  |= 0x00000008 ; // Enable the clock for port D
  while((SYSCTL->PRGPIO& 0x8) == 0){}
SYSCTL->RCGCI2C   |= 0x00000008 ; // Enable the clock for I2C3
	while((SYSCTL->PRI2C & 0x8) == 0){}
	
GPIOD->DEN |= 0x03; // Assert DEN for port D 0-1
// Configure Port B pins 2 and 3 as I2C3
GPIOD->AFSEL |= 0x03 ;
GPIOD->AMSEL &=~0x03; // Assert DEN for port D
GPIOD->PCTL &= ~0x000000FF ;
GPIOD->PCTL |= 0x00000033 ;
GPIOD->ODR|=0x02;


I2C3->MCR  = 0x0010 ; // Enable I2C3 master function

// I2C3->MTPR  = 0x07 ; //100khz
//I2C3->MTPR  = 0x03 ; //Clock freq is 200khz we can get up to 400khz with i2c normal mode
I2C3->MTPR  = 0x01 ; //Clock freq is 400khz for I2C
}

// Receive one byte of data from I2C slave device
char I2C3_Write(int slave_address, char slave_memory_address, int bytes_count, char* data)
{   
    char error;
    if (bytes_count <= 0)
        return -1;                  //If there is no data to be sent return error
    //Starting
    I2C3->MSA = slave_address << 1;
    I2C3->MDR = slave_memory_address;
    I2C3->MCS |= 3;                  //Start->SlaveAddress->ACK->SlaveMemoryAddress
	

    while(I2C3->MCS & 1){};   //wait for ack
    error= I2C3->MCS & 0xE; //Return error if there is an error
    if (error) return error;

    //If there is more than one data to send write ACK after every data until it is done
    while (bytes_count > 1)
    {
        I2C3->MDR = *data++;             //Data send through MDR
        I2C3->MCS = 1;                   // Send ACK
        while(I2C3->MCS & 1){};   //wait for ack
        error= I2C3->MCS & 0xE; //Return error if there is an error
        if (error) return error;
        bytes_count--;
    }
    
    //Last Byte
    I2C3->MDR = *data++;                 //Last data send
    I2C3->MCS = 5;                       //ACK after stop
    while(I2C3->MCS & 1){};   //wait for ack
    error= I2C3->MCS & 0xE; //Return error if there is an error
    while(I2C3->MCS & 0x40);             //Wait until transaction stops and line is not busy 
    if (error) return error;
    return 0;       /* no error */
}
/* This function reds from slave memory location of slave address */
/* read address should be specified in the second argument */
/* read: S-(saddr+w)-ACK-maddr-ACK-R-(saddr+r)-ACK-data-ACK-data-ACK-...-data-NACK-P */
char I2C3_Read(int slave_address, char slave_memory_address, int bytes_count, char* data)
{
    char error;
    
    if (bytes_count <= 0)
        return -1;         // no byte to be read so error (should be bytes_count>1)

    //Start->SlaveAddress->SlaveMemoryAdress->ACK->RESTART->SlaveAddress+1(ReadOp)
    I2C3->MSA = slave_address << 1;
    I2C3->MDR = slave_memory_address;
    I2C3->MCS = 3;      
    while(I2C3->MCS & 1){};   //wait for ack
    error= I2C3->MCS & 0xE; //Return error if there is an error
    if (error)
        return error;

   
    I2C3->MSA = (slave_address << 1) + 1;   //RESTART->SlaveAddress+1(ReadOp)

    if (bytes_count == 1)             //if single read send ack
        I2C3->MCS = 7;              
    else                           
        I2C3->MCS = 0xB;           //if more than one send nack
    while(I2C3->MCS & 1){};   //wait for ack
    error= I2C3->MCS & 0xE; //Return error if there is an error
    if (error) return error;

    *data++ = I2C3->MDR;            //Storing data 

    if (--bytes_count == 0)           
    {
        while(I2C3->MCS & 0x40);    
        return 0;       
    }
 
    
    while (bytes_count > 1)
    {
        I2C3->MCS = 9;              //Send ACK
        while(I2C3->MCS & 1){};   //wait for ack
        error= I2C3->MCS & 0xE; //Return error if there is an error
        if (error) return error;
        bytes_count--;
        *data++ = I2C3->MDR;        //Storing data
    }

    I2C3->MCS = 5;                  //NACK
    while(I2C3->MCS & 1){};   //wait for ack
    error= I2C3->MCS & 0xE; //Return error if there is an error
    *data = I2C3->MDR;              //Store last data
    while(I2C3->MCS & 0x40);        
    
    return 0;      
}