@**************************************
@author:kungfu_monkey
@filename:start.S
@**************************************
.text
.global _start
.global irq
_start:
	b reset
	ldr pc,_undefined_instruction								@设置中断向量表
	ldr pc,_software_interrupt
	ldr pc,_prefetch_abort
	ldr pc,_data_abort
	ldr pc,_not_used
	ldr pc,_irq
	ldr pc,_fiq

_undefined_instruction: .word undefined_instruction
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort
_not_used: .word not_used
_irq: .word irq
_fiq: .word fiq

undefined_instruction:
	nop
software_interrupt:
	nop
prefetch_abort:
	nop
data_abort:
	nop
not_used:
	nop
irq:
	sub lr,lr,#4
	stmfd sp!,{r0-r12,lr}
	bl irq_handler
	ldmfd sp!,{r0-r12,pc}^
fiq:
	nop
reset:
	bl set_svc
	bl disable_watchdog
	bl disable_interrupt
	bl disable_mmu
	bl clock_init  
	bl mem_init
	bl stack_init
	bl nand_init
@	bl iram_to_ram
	bl copy_to_ram	
	bl bss_clean
@	ldr pc,=mmu_demo
  ldr pc,=main
@	bl led_debug
	
@halt:											@让mcu一直执行这个语句  防止mcu跑飞
@	b halt

set_svc:
	mrs r0,cpsr
	bic r0,r0,#0xff				
	orr r0,r0,#0xd3						@禁止irq,fiq,设置为svc模式
	msr cpsr_c,r0
	mov pc,lr

#define  pwtcon 0xe2700000	
disable_watchdog:					  @关闭看门狗
	ldr r0,=pwtcon
	mov r1,#0x0
	str r1,[r0]
	mov pc,lr
	
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

disable_mmu:
	mcr p15,0,r0,c7,c5,0
	mcr p15,0,r0,c7,c6,1
	mrc p15,0,r0,c1,c0,0					@关闭MMU，关闭dcache(Icache没有关闭)
	bic r0, r0, #0x00000007
	mcr p15,0,r0,c1,c0,0
	mov pc, lr
	
#define	APLL_LOCK 0xe0100000
#define	MPLL_LOCK 0xe0100008
#define CLK_DIV0 0xe0100300
#define MPLL_CON 0xe0100108
#define APLL_CON 0xe0100100
#define CLK_SRC  0xe0100200

#define DIV_VAL ((0x0<<0)|(4<<4)|(4<<8)|(1<<12)|(3<<16)|(1<<20)|(4<<24)|(1<<28))
#define APLL_VAL ((1<<31)|(3<<8)|(125<<16)|(1<<0))
#define MPLL_VAL ((1<<31)|(12<<8)|(667<<16)|(1<<0))


clock_init:
	ldr r0,=APLL_LOCK										@产生稳定的时钟输入
	ldr r1,=0xfff
	str r1,[r0]
	
	ldr r0,=MPLL_LOCK
	ldr r1,=0xfff
	str r1,[r0]

	ldr r0, =CLK_DIV0										@设置分频比例
	ldr r1, =DIV_VAL
	str r1, [r0]
	
	ldr r0, =APLL_CON                   @APLL设置为1000MHz
	ldr r1, =APLL_VAL
	str r1, [r0]
	
	ldr r0, =MPLL_CON										@MPLL设置为667MHz
	ldr r1, =MPLL_VAL
	str r1, [r0]
	
	ldr r0, =CLK_SRC										@时钟源选择  使用晶振还是PLL
	ldr r1, =0x10001111											
	str r1, [r0]
	
	mov pc, lr
	
	
iram_to_ram:								@从iram复制到内存dram中运行
	adr r0, _start						@_start当前所在位置，为0xd0020010 
	ldr r1,=_start						@_start的链接地址，为0x20008000
	ldr r3,=bss_start           @bin文件最后的位置,bin文件中不需要保存bss段
	cmp r0,r1
	moveq pc,lr
	
copy_loop:
	ldr r2,[r0],#4						@把iram内容赋给r2,然后自加4     r2=[r0],r0=r0+4
	str r2,[r1],#4						@[r1]=r2,r1=r1+4
	cmp r1,r3
	bne copy_loop
		
	mov pc,lr

stack_init:
	msr cpsr_c, #0xd2
	ldr sp, =0x3e000000				@此处实际设置的是r13_irq,内存范围为0x2000_0000~0x3fff_ffff			
	msr cpsr_c, #0xd3
	ldr sp, =0x3f000000				@此处实际设置的是r13_svc
	mov pc, lr

copy_to_ram:
	mov r0,#0									@r0,r1,r2作为函数nand_to_ram的3个参数
	ldr r1,=_start
	ldr r2,=bss_start
	sub r2,r2,r1
	sub r1,r1,#0x10           @前16byte为加头放在_start-16~_start上，以保证代码位置对齐，从_start开始

	
	mov ip,lr
	bl nand_to_ram
	mov lr,ip
	
	mov pc,lr


bss_clean:
	ldr r0,=bss_start					@清除栈
	ldr r1,=bss_end
	cmp r0,r1
	moveq pc,lr
	

clean_loop:
	mov r2,#0
	str r2,[r0],#4
	cmp r0,r1
	bne clean_loop
	
	mov pc,lr


led_debug:          
	ldr	r0,=0xe0200280      @ GPJ2CON控制寄存器
	ldr	r1,=0x00001010
	str	r1,[r0]             @ 设置GPJ2_0，GPJ2_1,GPJ2_2,GPJ2_3为输出口, 位[3:0]=0b0001对应GPJ2_0
	
	ldr	r0,=0xe0200284      @ GPJ2DAT数据寄存器
	mov	r1,#0x0             @ 低电平点亮                       
	str	r1,[r0]  
	          
	mov pc,lr
	


