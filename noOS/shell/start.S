/*
 * 文件名：	start.S
 * 作者：	朱老师
 * 描述：	演示串口通信
 */

#define WTCON		0xE2700000

#define SVC_STACK	0xd0037d80
#define IRQ_STACK	0xd0037f80
#define CR_V (1<<13)
.global IRQ_handle
.global _start	

	
// 把_start链接属性改为外部，这样其他文件就可以看见_start了
   /* 
_start:                                 @ 程序入口标志  
    b   reset                       @ reset 复位异常  
        ldr pc, _undefined_instruction  @ 未定义异常, 将 _undefined_instruction 值装载到 pc 指针中  
        ldr pc,  _software_interrupt
       // ldr pc, _software_interrupt     @ 软中断异常  
        ldr pc, _prefetch_abort         @ 预取指令异常  
        ldr pc, _data_abort             @ 数据读取异常  
        ldr pc, _not_used               @ 占用 0x00000014 地址                            
        ldr pc, _irq                    @ 普通中断异常  
        ldr pc, _fiq                    @ 软中断异常  

reset:
*/
_start: 
  //
        mrs r0, cpsr
	bic r0 ,r0,#0x1f
	orr r0,r0,#0xd3
	msr cpsr,r0

/*
	mrc p15,0,r0,c1,c0,0
	bic r0,#CR_V
	mcr p15,0,r0,c1,c0,0
	ldr r0 ,=_start
	mcr p15,0,r0,c12,c0,0

 
 */
	ldr r0, =0xE010E81C
	ldr r1, [r0]
	ldr r2, =0x301
	orr r1, r1, r2
	str r1, [r0]

	// 第1步：关看门狗（向WTCON的bit5写入0即可）
	ldr r0, =WTCON
	ldr r1, =0x0
	str r1, [r0]
	bl disable_interrupt
        bl disable_mmu
	
        // 第2步：初始化时钟
	bl clock_init
	bl buzzer_init
	// 第3步：设置SVC栈
	ldr sp, =SVC_STACK
	
	// 第4步：开/关icache
	mrc p15,0,r0,c1,c0,0;			// 读出cp15的c1到r0中
	//bic r0, r0, #(1<<12)			// bit12 置0  关icache
	orr r0, r0, #(1<<12)			// bit12 置1  开icache
	mcr p15,0,r0,c1,c0,0;

   
	bl sdram_asm_init

  	bl buzzer_init
	//mov  r0, #1                  // 根据ATPCS规则，r0存放第一个参数
        //swi   1  

        // ldr  r0 ,=0x12345
      //  ldr r1 ,=0x20000080
      // str r0,[r1]
    

       // bl stack_init
/*
  */      	
        adr r0, _start
        ldr r1, =_start         
        ldr r2, =bss_start      
        cmp r0, r1                     
        beq clean_bss    
	bl copy_bl2_2_ddr
/*
copy_loop:
        ldr r3, [r0], #4    
        str r3, [r1], #4       
        cmp r1, r2                      
        bne copy_loop

*/
       
clean_bss:
        ldr r0, =bss_start
        ldr r1, =bss_end
        cmp r0, r1                              
        beq run_on_dram                 
        mov r2, #0
clear_loop:
        str r2, [r0], #4                
        cmp r0, r1                              
        bne clear_loop
run_on_dram:
        //ldr r0,=fmt
        //mov  r1, #8
        //bl printf
    
	//bl buzzer_on           // 
        ldr pc, =main   
		
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


_undefined_instruction: .word undefined_instruction @ _undefined_instruction 标号存放了一个值, 该值是 32 位地址 undefined_instruction, undefined_instruction 是一个地址  
_software_interrupt:    .word software_interrupt    @ 软中断异常  
_prefetch_abort:    .word prefetch_abort            @ 预取指令异常 处理  
_data_abort:        .word data_abort                @ 数据读取异常  
_not_used:      .word not_used                      @ 空位处理  
 
_irq:           .word irq              @ 普通中断处理  
_fiq:           .word irq                    @ 快速中断处理
undefined_instruction:                              @ undefined_instruction 地址存放要执行的内容  
        nop  

software_interrupt:                                 @ software_interrupt 地址存放要执行的内容  
       ldr sp, =0x20004000
       //bl led_blink
	STMFD     SP!, {R0-R12,  LR}           // ; 保存程序执行现场
         LDR R4, [LR, #-4]                          //         ; LR - 4 为指令" swi xxx" 的地址，低24位是软件中断号
         BIC   R4, R4, #0xFF000000                  //  ; 取得ARM指令24位立即数
         
         CMP          R4, #1                          //          ; 判断24位立即数，如果为1，调用do_led_on系统调用
         LDREQ     LR, =swi_return                  //  ; 软中断处理返回地址
   
 // LDREQ     PC, = do_led_on                  //  ; 软中断号1对应系统调用处理
      //   CMP          R4, #2                          //          ; 判断24位立即数，如果为2，调用do_led_off系统调用
      //   LDREQ     LR, =swi_return                  //  ; 软中断处理返回地址
      //   LDREQ     PC, = do_led_off                   //         ; 软中断号2对应系统调用处理
           
 
         MOVNE    R0, #-1                              //     ; 没有该软中断号对应函数，出错返回-1
 
swi_return:
 
         LDMIA     SP!, {R0-R12, PC}^ 

prefetch_abort:                                     @ prefetch_abort 地址存放要执行的内容  
        nop  

data_abort:                                         @ data_abort 地址存放要执行的内容  
        nop  

not_used:                                           @ not_used 地址存放要执行的内容  
        nop  
// 在这个汇编函数中，用来做中断模式下的现场保护和恢复，并且调用真正的中断处理程序

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
	
irq: b irq
	
	

	
	
	
	
	
	
	
	
	
