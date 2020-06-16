.file "DS"
.init main

.int64 ADDRESS_ONE 0            ; Index: 0
.int64 ADDRESS_TWO 0            ; Index: 8
.int64 DATA_0      27182818284  ; Data to store Index : 16
.int64 DATA_CANARY 9999999999   ; To make sure we arent having 2 wrongs make a right
.int64 ADDRESS_THREE 0          ; Index: 32

<main:

    call alloc          ; Call alloc. New address should be in r0
    stw $0(gs) r0       ; Store in GS ADDRESS_ONE

    call alloc          ; Call alloc. New address should be in r0
    stw $8(gs) r0       ; Store in GS ADDRESS_TWO

    call store          ; Store DATA_0 into ADDRESS_ONE
    
    call load_dump      ; Push data from storage onto GS

    call load_spec      ; Load into a specific region of the GS

    call copy           ; Allocates a new address and copys data to it from existing item in DS

    ldw r0 $0(gs)       ; Load ADDRESS_ONE for free
    call free           ; Free address in r0

    ldw r0 $8(gs)       ; Load ADDRESS_TWO for free
    call free           ; Free address in r0
>

<alloc:
    lsh r0 $13 $56      ; Move DS id into position
    
    mov r1 $8           ; We want 8 bytes
    lsh r1 r1 $16       ; Move value into place 

    or r10 r0 r1        ; Or together to create command into r10 to trigger execution

    mov r9 $1
    beq r11 r9 failed   ; Check for failure

    mov r0 r12          ; Move address into r0
    ret 

failed:
    exit
>

<store:
    ldw r11 $0(gs)      ; Load ADDRESS_ONE to destination

    lsh r0 $16 $32     ; Load start address for data
    lsh r1 $24 $0      ; Load end address for data
    or r12 r0 r1       ; Or together into register for pick up 

    lsh r0 $13 $56      ; Move DS id into position
    lsh r1 $10 $48      ; Move sub-id for 'store' into position
    or r10 r0 r1        ; Or command into trigger register

    mov r9 $1
    beq r11 r9 failed   ; Check for failure
    ret 
failed:
    exit
>

<load_dump:
    ldw r11 $0(gs)      ; Load ADDRESS_ONE to destination

    lsh r0 $13 $56      ; Move DS id into position
    lsh r1 $20 $48      ; Move sub-id for 'load' into position
    lsh r2 $1  $40      ; Move load type 'dump' into position

    or r1  r1 r2 
    or r10 r0 r1        ; Or into position

    mov r9 $1
    beq r11 r9 failed   ; Check for failure

    popw r0 gs          ; Get result - SHould be 271828.. not the 999999.. canary

    ret 
failed:
    exit
>

<load_spec:

    size r0 gs          ; Get current size

    lsh r12 r0 $32      ; Store as start index

    add r0 r0 $8        ; Add 8 to index
    or r12 r12 r0       ; Or start and end together to get range for op

    mov r0 $0           ; Push a 0 onto the stack - this is where we are instructing the thing to place the data
    pushw gs r0 

    ldw r11 $0(gs)      ; Load ADDRESS_ONE to destination

    lsh r0 $13 $56      ; Move DS id into position
    lsh r1 $20 $48      ; Move sub-id for 'load' into position
    lsh r2 $0  $40      ; Move load type 'specific' into position

    or r1  r1 r2 
    or r10 r0 r1        ; Or into position

    mov r9 $1
    beq r11 r9 failed   ; Check for failure

    popw r0 gs          ; Get result - SHould be 271828.. not the 999999.. canary

    ret 
failed:
    exit
>

<copy:

    call alloc          ; Call alloc. New address should be in r0
    stw $32(gs) r0       ; Store in GS ADDRESS_THREE

    mov r12 r0          ; Mark it as destination address

    ldw r11 $0(gs)      ; Load ADDRESS_ONE into source address
    
    stw $0(gs) r12     ; Overwrite ADDRESS_ONE as new address so we can utilize load after copy

    lsh r0 $13 $56      ; Move DS id into position
    lsh r1 $5  $48      ; Move sub-id for 'copy' into position
    or  r10 r0 r1       ; Call copy

    mov r9 $1
    beq r11 r9 failed   ; Check for failure


    call load_dump      ; Should be 271828..


    ret 
failed:
    exit
>

<free:
    mov r11 r0          ; Move address into r11

    lsh r0 $13 $56      ; Move DS id into position
    lsh r1 $1  $48      ; Move sub-id for 'free' into position
    or r10 r0 r1        ; Or together to create command into r10

    mov r9 $1
    beq r11 r9 failed   ; Check for failure
    ret 
failed:
    exit
>

