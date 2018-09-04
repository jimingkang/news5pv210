abc:
mov ip,sp
stmfd sp!,{fp,ip,lr,pc}
sub fp,ip,#4

@ldr r0,=test
mov lr,pc
ldr pc,test
@mov pc,r0

sub sp ,fp,#12
ldmfd sp,{fp,sp,pc}

test:
mov ip,sp
sub sp,sp,#12
str r14,[sp]
str ip,[sp,#4]
str fp,[sp,#8]
sub fp,ip,#4

ldr r0,=fmt
mov r14,r15
ldr r15,show

ldr lr,[fp,#-8]
ldr ip,[fp,#-4]
ldr fp,[fp,#0]
mov sp,ip
mov r15,r14

show:
     .word 0xc3e1528c
fmt:
    .asciz "hello!\n"
fmt2:
    .asciz "world!\n"
