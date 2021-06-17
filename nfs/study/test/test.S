ldr r5,=0x32000000
str r14,[r5]
ldr r0,=fmt
mov r14,r15
ldr r15,show
ldr r14,[r5]
mov r15,r14

show:
     .word 0xc3e1528c
fmt:
    .asciz "hello!\n"
