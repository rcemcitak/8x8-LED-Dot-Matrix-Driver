; 5) REFRESH.s file
;
			AREA	chars_, DATA, READONLY, ALIGN=2
MINUS		DCB		"-"
			DCB		0x04
PLUS		DCB		"+"
			DCB		0x04
ADDRES4		EQU 	0x20001018

			AREA 	routines, CODE, READONLY
			THUMB
			
			IMPORT	SSI_WRITE
			IMPORT 	CONVRT
			IMPORT	SEND_STRDATA
			IMPORT	MAINS
			EXPORT	REFRESH
				
REFRESH		PROC
			
			PUSH	{R0-R2, LR}
			
			MOV		R9,#0xBA		;Starting point of X
			BL		SSI_WRITE
			MOV		R9,#0x40		;Y=0
			BL		SSI_WRITE
			MOV		R9,#0x100		;Clear screen
			MOV		R0,#18
clearX		BL		SSI_WRITE
			SUBS	R0,#1
			BNE		clearX
			
			
			MOV		R9,#0xBA		;Starting point of X
			BL		SSI_WRITE
			MOV		R9,#0x41		;Y=1
			BL		SSI_WRITE
			MOV		R9,#0x100		;Clear screen
			MOV		R0,#18
clearY		BL		SSI_WRITE
			SUBS	R0,#1
			BNE		clearY
			
			MOV		R9,#0xBA		;Starting point of X
			BL		SSI_WRITE
			MOV		R9,#0x42		;Y=2
			BL		SSI_WRITE
			MOV		R9,#0x100		;Clear screen
			MOV		R0,#18
clearZ		BL		SSI_WRITE
			SUBS	R0,#1
			BNE		clearZ
			
			; X Axis Angle
			MOV		R9,#0xBF		;Reset X position
			BL		SSI_WRITE
			MOV		R9,#0x40		;Vertical=0
			BL		SSI_WRITE
			
			MOV		R0,R7
			BL		CONVRT
			MOV		R0,R10
			BL		SEND_STRDATA
			
			; Checking the sign
			CMP		R3, #1
			BNE		yaxis ; if the angle is negative, write a minus sign
			MOV		R9,#0xBA		;Reset X position
			BL		SSI_WRITE
			MOV		R9,#0x40		;Vertical=0
			BL		SSI_WRITE
			LDR		R0,=MINUS
			BL		SEND_STRDATA
									
			; Y Axis Angle
yaxis		MOV		R9,#0xBF		;Starting point of X
			BL		SSI_WRITE
			MOV		R9,#0x41		;Vertical=1
			BL		SSI_WRITE
			
			MOV		R0,R8
			BL		CONVRT
			MOV		R0,R10
			BL		SEND_STRDATA
			
			; Checking the sign
			LDR		R4, =ADDRES4
			LDR		R5, [R4] ; Y angle sign
			CMP		R5, #1
			BNE		zaxis ; if the angle is negative, write a minus sign
			MOV		R9,#0xBA		;Reset X position
			BL		SSI_WRITE
			MOV		R9,#0x41		;Vertical=0
			BL		SSI_WRITE
			LDR		R0,=MINUS
			BL		SEND_STRDATA
			
			; Z Axis Angle
zaxis		MOV		R9,#0xBF		;Starting point of X
			BL		SSI_WRITE
			MOV		R9,#0x42		;Vertical=2
			BL		SSI_WRITE
			
			MOV		R0,R11
			BL		CONVRT
			MOV		R0,R10
			BL		SEND_STRDATA
			
			; Checking the sign
			CMP		R6, #1
			BNE		finish ; if the angle is negative, write a minus sign
			MOV		R9,#0xBA		;Reset X position
			BL		SSI_WRITE
			MOV		R9,#0x42		;Vertical=0
			BL		SSI_WRITE
			LDR		R0,=MINUS
			BL		SEND_STRDATA
			
finish		NOP
            
			POP		{R0-R2, LR}
			BX		LR
			
			ENDP
			ALIGN
			END
			