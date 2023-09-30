#define  GPJ2CON   (*(volatile unsigned long*) 0xA0000280)
#define  GPJ2DAT	 (*(volatile unsigned long*) 0xA0000284)

#define  SECTION_FLAG         0x2
#define  SECTION_BUFFERABLE   0x1<<2
#define  SECTION_CACHABLE     0x1<<3
#define  SECTION_SPECIAL	    0x1<<4
#define  SECTION_DOMAIN       0x0<<5    /*���ڵ�0����*/
#define  SECTION_AP_WR        0x3<<10   /*��Ȩ�����û�������Ȩ�޾�Ϊ�ɶ���д*/

#define  MMU_SECTION_DES_WR			(SECTION_FLAG|SECTION_BUFFERABLE|SECTION_CACHABLE|SECTION_SPECIAL|SECTION_DOMAIN|SECTION_AP_WR)
#define  MMU_SECTION_DES        (SECTION_FLAG|SECTION_SPECIAL|SECTION_DOMAIN|SECTION_AP_WR)

void create_page_table()
{
	unsigned long *ttb=(unsigned long*)0x20000000;
	
	unsigned long vaddr,paddr;
		
	vaddr=0xA0000000;
	paddr=0xE0200000;
	
	//*(ttb + ((vaddr&0xfff00000)>>18))=(paddr&0xfff00000)|MMU_SECTION_DES;
	*(ttb + (vaddr >> 20))=(paddr&0xfff00000)|MMU_SECTION_DES;
	
	
	vaddr=0x20000000;
	paddr=0x20000000;
	while(vaddr<0x24000000)
	{
		//*(ttb + ((vaddr&0xfff00000)>>18))=(paddr&0xfff00000)|MMU_SECTION_DES;
		*(ttb + (vaddr >> 20))=(paddr&0xfff00000)|MMU_SECTION_DES_WR;
		vaddr+=0x100000;                     //��ӳ��1MB���ڴ��
		paddr+=0x100000;
	}	
}


void  mmu_enable()
{
	__asm__(
		/*����ttb*/
		"ldr r0,=0x20000000\n"
		"mcr p15,0,r0,c2,c0,0\n"
		
		/*������ķ���Ȩ��,����Ϊȫ1�������Ƿ���Ȩ��*/
		"mvn r0,#0\n"
		"mcr p15,0,r0,c3,c0,0\n"
		
		/*ʹ��MMU*/
		"mrc p15,0,r0,c1,c0,0\n"
		"orr r0,r0,#0x1\n"
		"mcr p15,0,r0,c1,c0,0\n"
		:
		:
	);
	
	
}


void mmu_init()
{
		//1.����ҳ��
	create_page_table();
	
	//2.����ttb,ʹ��MMU
	mmu_enable();	
	
	
}


int mmu_demo()
{
	mmu_init();
	
	GPJ2CON=0x1|(0x1<<4)|(0x1<<8)|(0x1<<12);
	GPJ2DAT=0xA;
	
	return 0;	
}