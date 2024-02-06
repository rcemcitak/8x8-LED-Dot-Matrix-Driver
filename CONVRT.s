; 6) CONVRT.s Subroutine
; 
			AREA		routines, READONLY, CODE
			THUMB
			
			IMPORT		MAINS
			EXPORT		CONVRT

CONVRT		PROC
			
			PUSH		{R1-R3, R10, LR}
			
			MOV			R3,#10		
			MOV			R1,#0		
			
takeone		UDIV		R2,R0,R3	
			MUL			R2,R3		
			SUB			R2,R0,R2	
			ADD			R2,#0x30	
			PUSH		{R2}
			UDIV		R0,R3		
			ADD			R1,#1		
			CMP			R0,#0		
			BNE			takeone

order		POP			{R2}

			STRB		R2,[R10],#1	
			SUBS		R1,#1		
			MOVEQ		R2,#0x04	
			STRBEQ		R2,[R10]
			BNE			order
			
			POP			{R1-R3, R10, LR}
			
			BX			LR
			
			ENDP
			ALIGN
			END
			