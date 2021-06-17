/*
 * 文件名：	led.s	
 * 作者：	朱老师
 * 描述：	演示重定位（在SRAM内部重定位）
 */

#define WTCON		0xE2700000

#define SVC_STACK	0xd0037d80
#define IRQ_STACK	0xd0037f80

.global _start	
.global IRQ_handle					// 把_start链接属性改为外部，这样其他文件就可以看见_start了
_start:

// 如果不清除bss，则（显式初始化为0和没有显式初始化的）全局变量没有初始化为0
// 这就是帮助C语言构建C运行时环境
clean_bss:
	ldr r0, =bss_start					
	ldr r1, =bss_end
	cmp r0, r1				// 如果r0等于r1，说明bss段为空，直接下去
	beq run_on_dram			// 清除bss完之后的地址
	mov r2, #0
clear_loop:
	str r2, [r0], #4		// 先将r2中的值放入r0所指向的内存地址（r0中的值作为内存地址），
	cmp r0, r1				// 然后r0 = r0 + 4
	bne clear_loop

run_on_dram:	
	// 长跳转到led_blink开始第二阶段
	ldr pc, =main				// ldr指令实现长跳转
	
	
// 汇编最后的这个死循环不能丢
	b .
	
IRQ_handle:
	// 设置IRQ模式下的栈
	ldr sp, =IRQ_STACK
	// 保存LR
	// 因为ARM有流水线，所以PC的值会比真正执行的代码+8，
	sub lr, lr, #4
	// 保存r0-r12和lr到irq模式下的栈上面
	stmfd sp!, {r0-r12, lr}
	// 在此调用真正的isr来处理中断
	bl irq_handler
	// 处理完成开始恢复现场,其实就是做中断返回，关键是将r0-r12，pc，cpsr一起回复
	ldmfd sp!, {r0-r12, pc}^	
	
	
	

	
	
	
	
	
	
	
	
	
