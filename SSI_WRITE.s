; 4) SSI_WRITE.s file
;
SSI0_DR				EQU		0x40008008 
SSI0_SR				EQU		0x4000800C 
GPIO_PORTA_DATA_DC	EQU		0x40004100 

			AREA 	routines, CODE, READONLY
			THUMB
			
			IMPORT	MAINS
			EXPORT	SSI_WRITE
			
SSI_WRITE	PROC	
			
			PUSH	{R0-R2}
			
			LDR		R1,=SSI0_SR	   
check		LDR		R0,[R1]
			BIC		R0,#0x0F
			CMP		R0,#0x10
			BEQ		check
			
			LDR		R1,=GPIO_PORTA_DATA_DC ; PA6 pin
			LDR		R0,[R1]
			MOV		R0,#0	   		
			
			MOV		R0,R9			
			LSR		R0,#2			
			STR		R0,[R1]		
						
			LDR		R1,=SSI0_DR		
			LDR		R0,[R1]
			MOV		R0,R9
			BIC		R0,#0xF00
			STR		R0,[R1]
			
			POP		{R0-R2}
			BX		LR
			ENDP
			
			ALIGN
			END
			