/*
 * 文件名：	led.s	
 * 作者：	朱老师
 * 描述：	演示重定位（在SRAM内部重定位）
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
	
	// 第3步：开/关icache
	mrc p15,0,r0,c1,c0,0;			// 读出cp15的c1到r0中
	//bic r0, r0, #(1<<12)			// bit12 置0  关icache
	orr r0, r0, #(1<<12)			// bit12 置1  开icache
	mcr p15,0,r0,c1,c0,0;
        

        bl disable_mmu
        bl disable_interrupt

        bl clock_init
	//bl uart_init
	// 第4步：初始化ddr
	bl sdram_asm_init
	
	bl led1
	bl delay
	
	// 第5步：重定位，从SD卡第45扇区开始，复制32个扇区内容到DDR的0x23E00000
	bl copy_bl2_2_ddr

	bl led2
	bl delay

// 汇编最后的这个死循环不能丢
	b .
	
disable_mmu:
	mcr p15,0,r0,c7,c5,0
	mcr p15,0,r0,c7,c6,1
	mrc p15,0,r0,c1,c0,0					@关闭MMU，关闭dcache(Icache没有关闭)
	bic r0, r0, #0x00000007
	mcr p15,0,r0,c1,c0,0
	mov pc, lr
stack_init:
	msr cpsr_c, #0xd2
	ldr sp, =0x2e000000				@此处实际设置的是r13_irq,内存范围为0x2000_0000~0x3fff_ffff			
	msr cpsr_c, #0xd3
	ldr sp, =0x2f000000				@此处实际设置的是r13_svc
	mov pc, lr
disable_interrupt:					@屏蔽中断
	mvn r0,#0x0             
	ldr r1,=0xf2000014
	str r0,[r1]
	
	ldr r1,=0xf2100014
	str r0,[r1]
	
	ldr r1,=0xf2200014
	str r0,[r1]
	
	ldr r1,=0xf2300014
	str r0,[r1]
	mov pc,lr


	
	
	

	
	
	
	
	
	
	
	
	
