; 2) CONFIG.s file
;
;GPIO_PORTA Registers
RCGCGPIO		EQU 	0x400FE608 
PORTA_DEN	    EQU 	0x4000451C 
PORTA_PCTL 		EQU 	0x4000452C 
PORTA_AFSEL 	EQU 	0x40004420 
PORTA_AMSEL 	EQU 	0x40004528 
PORTA_DATA		EQU		0x400043FC 
PORTA_DIR		EQU		0x40004400 
PORTA_PUR		EQU		0x40004510 

;SSI0 Registers
RCGCSSI			EQU		0x400FE61C 
SSI0_CR0		EQU		0x40008000 
SSI0_CR1		EQU		0x40008004 
SSI0_CPSR		EQU		0x40008010 
SSI0_CC			EQU		0x40008FC8 

			AREA	chars_, DATA, READONLY, ALIGN=2
X_ANGLE		DCB		"X ANGLE: "
			DCB		0x04	

Y_ANGLE		DCB		"Y ANGLE: "
			DCB		0x04

Z_ANGLE		DCB		"Z ANGLE: "
			DCB		0x04

			AREA 	routines, CODE, READONLY
			THUMB

			IMPORT	SEND_STRDATA
			IMPORT 	SSI_WRITE
			IMPORT	MAINS
			EXPORT	CONFIG
CONFIG		PROC	
			PUSH	{R0-R2,LR}
			
			LDR		R1,=RCGCGPIO
			LDR		R0,[R1]
			ORR		R0,R0,#0x01 	;Enable CLK on A
			STR		R0,[R1]
			
			NOP		; Instead of PRGPIO
			NOP
			NOP
			
			LDR		R1,=RCGCSSI
			LDR		R0,[R1]
			ORR		R0,R0,#0x01		;Enable CLK on SSI0
			STR		R0,[R1]
			
			NOP
			NOP
			NOP
			
			LDR		R1,=PORTA_DIR
			LDR		R0,[R1]
			ORR		R0,#0xC0		;A6,A7 outputs
			STR		R0,[R1]
			
			LDR		R1,=PORTA_DEN
			LDR		R0,[R1]
			ORR		R0,#0xFF		;All digital
			STR		R0,[R1]
			
			LDR		R1,=PORTA_AFSEL
			LDR 	R0,[R1]
			ORR		R0,#0x2C		;A2,A3,A5 alternating func
			STR		R0,[R1]
		
			LDR		R1,=PORTA_PCTL
			LDR		R0,[R1]
			MOV		R0,#0x2022
			LSL		R0,#8			;#0x00202200	,SSI alt functions for A2,A3,A5
			STR		R0,[R1]
			
			LDR		R1,=PORTA_PUR
			LDR		R0,[R1]
			ORR		R0,#0x04		;Set CLK pin of LCD (PA2) as PUR due to steady state low in CR0
			STR		R0,[R1]
			
			LDR		R1,=SSI0_CR1
			LDR		R0,[R1]
			BIC		R0,#0x02		;Disable serial port
			STR		R0,[R1]
			BIC		R0,#0x04		;After SSE=0, select as master
			STR		R0,[R1]
			
			;Configure SSI0Clk to 2.6667 MHz = 16 MHz/(2*(1+2))
			;BR = SYSCLK / (CSDVSR * (1+SCR))
			LDR		R1,=SSI0_CC
			LDR		R0,[R1]
			MOV		R0,#0x00		;Use system clk, 16MHz
			STR		R0,[R1]
			
			LDR		R1,=SSI0_CPSR
			LDR		R0,[R1]
			MOV		R0,#2			;Divide clock by prescaling
			STR		R0,[R1]
			
			LDR		R1,=SSI0_CR0
			LDR		R0,[R1]
			MOV		R0,#0x207		;SCR=2 for prescaling
									;1st edge transition (rising), steady state low, freescale SPI frame
									;8-bit data
			STR		R0,[R1]
			
			LDR		R1,=SSI0_CR1
			LDR		R0,[R1]
			ORR		R0,#0x02		;Enable serial port
			STR		R0,[R1]
						
			LDR		R1,=PORTA_DATA
			LDR		R0,[R1]
			BIC		R0,#0x80		;RST pin PA7 to LOW
			STR		R0,[R1]
			
			MOV32	R2,#300000		;Delay
loop		SUBS	R2,#1
			BNE		loop			
			
			ORR		R0,#0x80		;RST pin PA7 to HIGH
			STR		R0,[R1]
			
			MOV		R9,#0x21		;Enable chip (PD=0), Horizontal addressing (V=0), Extended instructions (H=1)	
			BL		SSI_WRITE
			
			MOV		R9,#0xBF		;VOP
			BL		SSI_WRITE
			
			MOV		R9,#0x06		;Temperature control
			BL		SSI_WRITE
			
			MOV		R9,#0x13		;Voltage bias
			BL		SSI_WRITE

			MOV		R9,#0x20		;H=0 for basic command mode
			BL		SSI_WRITE
			
			MOV		R9,#0x0C		;Normal display mode
			BL		SSI_WRITE
			
			MOV		R9,#0x80		;Reset X position
			BL		SSI_WRITE
			MOV		R9,#0x40		;Reset Y position
			BL		SSI_WRITE
			
			MOV		R9,#0x100		;Clear screen
			MOV		R0,#504
clear		BL		SSI_WRITE
			SUBS	R0,#1
			BNE		clear

			MOV		R9,#0x80		;Reset X position
			BL		SSI_WRITE
			MOV		R9,#0x40		;Y=0
			BL		SSI_WRITE
			
			LDR		R0,=X_ANGLE
			BL		SEND_STRDATA

			MOV		R9,#0x80		;Reset X position
			BL		SSI_WRITE
			MOV		R9,#0x41		;Y=1
			BL		SSI_WRITE
			
			LDR		R0,=Y_ANGLE
			BL		SEND_STRDATA

			MOV		R9,#0x80		;Reset X position
			BL		SSI_WRITE
			MOV		R9,#0x42		;Y=2
			BL		SSI_WRITE
			
			LDR		R0,=Z_ANGLE
			BL		SEND_STRDATA			
			
			POP		{R0-R2,LR}
			BX 		LR
			ENDP
			ALIGN
			END
