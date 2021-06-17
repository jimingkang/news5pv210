/*
 * 文件名：	led.s	
 * 作者：	朱老师
 * 描述：	演示汇编设置栈并且调用C语言程序来点亮LED
 */

#define WTCON		0xE2700000

#define SVC_STACK	0xd0037d80

.global _start					// 把_start链接属性改为外部，这样其他文件就可以看见_start了
_start:
	// 第1步：关看门狗（向WTCON的bit5写入0即可）
	ldr r0, =WTCON
	ldr r1, =0x0
	str r1, [r0]
	
	// 第2步：设置SVC栈
	ldr sp, =SVC_STACK

	// 从这里之后就可以开始调用C程序了
	bl led_blink					// led_blink是C语言实现的一个函数
	
// 汇编最后的这个死循环不能丢
	b .
	

	
	
	

	
	
	
	
	
	
	
	
	
