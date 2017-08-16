; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB
	PRESERVE8 

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec

	PUSH {R4, LR}		;using R4 and LR
	
	MOV R4, #0
	MOV R1, #10
OUTD_LOOP 
	UDIV R2, R0, R1		;loop and push all numbers onto stack
	MUL R3, R2, R1
	ADDS R3, #0
	BEQ OUTD_DONE
	SUB R0, R0, R3
	PUSH {R0}
	MOV R0, R2
	ADD R4, #1
	BL OUTD_LOOP
	
OUTD_DONE 	
	ADD R0, #0x30
	BL ST7735_OutChar
	CMP R4, #0
	BEQ REALLY_DONE	

OUTD_OUTPUT	
	POP {R0}				;loop and pop all numbers off of stack
	ADD R0, #0x30
	BL ST7735_OutChar		;output each number one by one
	SUBS R4, #1
	BNE OUTD_OUTPUT
 
	
REALLY_DONE 
	POP {R4, LR}

    BX LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix



	PUSH {LR}		;using LR and R4
	PUSH {R4}
	MOV R4, #0
	MOV R1, #10000
	CMP R0, R1
	BHS TOOBIG		; branch higher or same
	
	MOV R1, #10
OUTF_LOOP 
	UDIV R2, R0, R1		;loop and push all numbers onto stack
	MUL R3, R2, R1
	ADDS R3, #0
	BEQ OUTF_DONE
	SUB R0, R0, R3
	PUSH {R0}
	MOV R0, R2
	ADD R4, #1
	BL OUTF_LOOP
	
	
	
TOOBIG 
	MOV R0, #0x2A			;star ASCII hex value
	BL ST7735_OutChar		;if the number is too big, output all stars
	MOV R0, #0x2E
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	POP {LR, R4}
	
	BX LR
	
OUTF_DONE
	MOV R1, #3
	CMP R1, R4			;determines the size of the number and branches
	BEQ FOUR
	MOV R1, #2
	CMP R1, R4
	BEQ THREE
	MOV R1, #1
	CMP R1, R4
	BEQ TWO
	MOV R1, #0
	CMP R1, R4
	BEQ ONE
	
THREE MOV R4, R0	; PARTIAL NUMBER, FORMAT: 0.XXX
	MOV R0, #0x30
	BL ST7735_OutChar		;first number is 0
	MOV R0, #0x2E
	BL ST7735_OutChar
	MOV R0, R4
	ADD R0, #0x30
	BL ST7735_OutChar		;convert to ASCII hex and output each number idividually
	POP {R0}
	ADD R0, #0x30
	BL ST7735_OutChar
	POP {R0}
	ADD R0, #0x30
	BL ST7735_OutChar
	POP {LR, R4}

    BX   LR
	
TWO MOV R4, R0		; PARTIAL NUMBER, FORMAT: 0.0XX
	MOV R0, #0x30
	BL ST7735_OutChar
	MOV R0, #0x2E
	BL ST7735_OutChar
	MOV R0, #0x30
	BL ST7735_OutChar		;first two numbers are 0s
	MOV R0, R4
	ADD R0, #0x30
	BL ST7735_OutChar	;convert to ASCII hex and output each number idividually
	POP {R0}
	ADD R0, #0x30
	BL ST7735_OutChar
	POP {LR, R4}
	
    BX   LR
	 
ONE MOV R4, R0		; PARTIAL NUMBER, FORMAT: 0.00X
	MOV R0, #0x30
	BL ST7735_OutChar
	MOV R0, #0x2E
	BL ST7735_OutChar		;first three numbers are 0s
	MOV R0, #0x30
	BL ST7735_OutChar
	MOV R0, #0x30
	BL ST7735_OutChar
	MOV R0, R4
	ADD R0, #0x30
	BL ST7735_OutChar		;convert to ASCII hex and output each number idividually
	POP {LR, R4}

    BX   LR
	 
FOUR 
	ADD R0, #0x30		; FULL NUMBER, FORMAT: X.XXX
	BL ST7735_OutChar		; FIRST NUMBER
	MOV R0, #0x2E
	BL ST7735_OutChar		; .
	POP {R0}
	ADD R0, #0x30
	BL ST7735_OutChar		; SECOND NUMBER
	POP {R0}
	ADD R0, #0x30
	BL ST7735_OutChar		; THIRD NUMBER
	POP {R0}
	ADD R0, #0x30
	BL ST7735_OutChar		; FOURTH (AND FINAL) NUMBER
	POP {LR, R4}
	



     BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
