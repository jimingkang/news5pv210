#define WTCON 0xE2700000
#define SVC_STACK 0xd0037d80
.global _start
_start:
ldr r0,=WTCON
ldr r1,=0x0
str r1,[r0]

ldr sp,=SVC_STACK

mrc p15,0,r0,c1,c0,0;
orr r0,r0,#(1<<12)
mcr p15,0, r0,c1,c0,0;
bl key_init
bl key_polling
b .

