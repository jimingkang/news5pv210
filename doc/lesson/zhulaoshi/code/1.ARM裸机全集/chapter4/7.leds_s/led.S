/*
 * 文件名：	led.s	
 * 作者：	朱老师
 * 描述：	流水灯
 */
 
#define GPJ0CON	0xE0200240
#define GPJ0DAT	0xE0200244

.global _start					// 把_start链接属性改为外部，这样其他文件就可以看见_start了
_start:
	// 第一步：把所有引脚都设置为输出模式，代码不变
	ldr r0, =0x11111111			// 从后面的=可以看出用的是ldr伪指令，因为需要编译器来判断这个数
	ldr r1, =GPJ0CON			// 是合法立即数还是非法立即数。一般写代码都用ldr伪指令
	str r0, [r1]				// 寄存器间接寻址。功能是把r0中的数写入到r1中的数为地址的内存中去

	// 要实现流水灯，只要在主循环中实现1圈的流水显示效果即可
flash:
	// 第1步：点亮LED1，其他熄灭
	ldr r0, =((0<<3) | (1<<4) | (1<<5))	// 清清楚楚的看到哪个灭，哪个是亮
	ldr r1, =GPJ0DAT
	str r0, [r1]				// 把0写入到GPJ0DAT寄存器中，引脚即输出低电平，LED点亮
	// 然后延时
	bl delay					// 使用bl进行函数调用
	
	// 第2步：点亮LED2，其他熄灭
	ldr r0, =((1<<3) | (0<<4) | (1<<5))	// 清清楚楚的看到哪个灭，哪个是亮
	ldr r1, =GPJ0DAT
	str r0, [r1]				// 把0写入到GPJ0DAT寄存器中，引脚即输出低电平，LED点亮
	// 然后延时
	bl delay	

	// 第3步：点亮LED3，其他熄灭
	ldr r0, =((1<<3) | (1<<4) | (0<<5))	// 清清楚楚的看到哪个灭，哪个是亮
	ldr r1, =GPJ0DAT
	str r0, [r1]				// 把0写入到GPJ0DAT寄存器中，引脚即输出低电平，LED点亮
	// 然后延时
	bl delay		
	
	
	b flash


// 延时函数：函数名：delay
delay:
	ldr r2, =9000000
	ldr r3, =0x0
delay_loop:	
	sub r2, r2, #1				//r2 = r2 -1
	cmp r2, r3					// cmp会影响Z标志位，如果r2等于r3则Z=1，下一句中eq就会成立
	bne delay_loop
	mov pc, lr					// 函数调用返回



	
	
	
	

	
	
	
	
	
	
	
	
	
