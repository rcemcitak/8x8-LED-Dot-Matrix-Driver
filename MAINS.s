; EE447 TERM PROJECT 
; NOKIA 5110 LCD SCREEN 
;
; 1) MAIN_S subroutine
;
ADDRES1		EQU 	0x20001000
ADDRES2		EQU 	0x20001008
ADDRES3		EQU 	0x20001010
ADDRES4		EQU 	0x20001018
ADDRES5		EQU 	0x20001020
ADDRES6		EQU 	0x20001028
ADDRES7		EQU 	0x20001030
STR_ADDR	EQU		0x20000804

			AREA	routines, READONLY, CODE
			THUMB
			
			IMPORT 	DELAY100
			EXTERN 	CONFIG
			EXTERN 	REFRESH
			EXPORT	MAINS
			
MAINS		PROC
			
START		MOV		R9,#0
			BL 		CONFIG
			
LOOP		LDR		R4, =ADDRES1
			LDR		R7, [R4] ; X angle magnitude
			
			LDR		R4, =ADDRES2
			LDR		R3, [R4] ; X angle sign
			
			LDR		R4, =ADDRES3
			LDR		R8, [R4] ; Y angle magnitude
			
			LDR		R4, =ADDRES4
			LDR		R5, [R4] ; Y angle sign
			
			LDR		R4, =ADDRES5
			LDR		R11, [R4] ; Z angle magnitude
			
			LDR		R4, =ADDRES6
			LDR		R6, [R4] ; Z angle sign
			
			LDR		R10,=STR_ADDR

			
			BL 		REFRESH
			BL 		DELAY100
			B		LOOP ; Enter a loop for the changing values of angles
			
			ENDP
			ALIGN
			END