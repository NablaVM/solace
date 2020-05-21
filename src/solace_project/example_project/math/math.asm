;
;	MODULUS
;	Performs modulus operation on r1, and r2
;	Results in r0
;	Ex:  r0 = r1 % r2
;	Uses : r0, r1, r2, r3, r4
;   Saves: r1, r2, r3, r4
<modulus:
    
    blt r1 r2 modspecial

    pushw ls r1
    pushw ls r2 
    pushw ls r3
    pushw ls r4

	mov r3 $0   		; Init temp, and result
	div r3 r1 r2		; Divide Num/Denom
    mov r4 $0
	beq r3 r4 moduiz	; Check if mod is 0
	jmp moduinz	    	; If not, go to moduinz
moduiz:
		add r0 $0 $0	; Indicate is 0
		jmp moddone
moduinz:
		mul r3 r2 r3 	; Mult denom by result
		sub r0 r1 r3	; Sub result from num
		jmp moddone
moddone:

    popw r4 ls 
    popw r3 ls 
    popw r2 ls 
    popw r1 ls 
    ret

modspecial:
    mov r0 r1
>