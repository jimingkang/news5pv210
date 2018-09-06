abc:
mov ip,sp
stmfd sp!,{fp,ip,lr,pc}
sub fp,ip,#4

ldr r0,=led
mov lr,pc
mov pc,r0

sub sp ,fp,#12
ldmfd sp,{fp,sp,pc}

led:
mov ip,sp
sub sp,sp,#12
str r14,[sp]
str ip,[sp,#4]
str fp,[sp,#8]
sub fp,ip,#4


	@ 第一步：把0x11111111写入0xE0200240(GPJ0CON)位置
	ldr r0, =0x11111111			@ 从后面的=可以看出用的是ldr伪指令，因为需要编译器来判断这个数
	ldr r1, =0xE0200240			@是合法立即数还是非法立即数。一般写代码都用ldr伪指令
	str r0, [r1]				@ 寄存器间接寻址。功能是把r0中的数写入到r1中的数为地址的内存中去

	@ 第二步：把0x0写入0xE0200244(GPJ0DAT)位置
	ldr r0, =((1<<3) | (0<<4) | (1<<5))  
	ldr r1, =0xE0200244
	str r0, [r1]			@ 把0写入到GPJ0DAT寄存器中，引脚即输出低电平，LED点亮


ldr r0,=fmt
ldr r1,=fmt
mov r2,pc
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
    .asciz "hello! fmt=%p\n pc=%p\n"
fmt2:
    .asciz "world!\n"
