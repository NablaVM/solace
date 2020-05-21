.file "Example_Project"

.init main

;
;
<main:


    ;   Print
    ;
    mul r0 $16 $200 ; Move has a 2^8 limit for raw numericals, so we leverage math here to get big numbs
    add r0 r0 $1
    call print_int_ascii
    call print_nl
>