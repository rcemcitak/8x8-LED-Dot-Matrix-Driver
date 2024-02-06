;3) DELAY100 Subroutine
			AREA    routines, READONLY, CODE
			THUMB
			EXPORT  DELAY100
			IMPORT 	MAINS

DELAY100	PROC
	
			PUSH	    {R0,LR}
			MOV32		R0,#7000000
loop		SUBS		R0,#1
			BNE			loop
			POP			{R0}
			BX			LR
			
			ENDP
			END