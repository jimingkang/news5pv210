#define SD_START_BLOCK	45
#define SD_BLOCK_CNT	(2048*2)
#define DDR_START_ADDR	0x23E00000



typedef unsigned int bool;

// ͨ���ţ�0������2
// ��ʼ�����ţ�45
// ��ȡ����������32
// ��ȡ������ڴ��ַ��0x23E00000
// with_init��0
typedef bool(*pCopySDMMC2Mem)(int, unsigned int, unsigned short, unsigned int*, bool);

typedef void (*pBL2Type)(void);


// ��SD����45������ʼ������32���������ݵ�DDR��0x23E00000��Ȼ����ת��23E00000ȥִ��
void copy_bl2_2_ddr(void)
{
	// ��һ������ȡSD��������DDR��
	pCopySDMMC2Mem p1 = (pCopySDMMC2Mem)(*(unsigned int *)0xD0037F98);
	//pCopySDMMC2Mem p1 = (pCopySDMMC2Mem)0xD0037F98);
	
	led2();
	delay();
	p1(2, SD_START_BLOCK, SD_BLOCK_CNT, (unsigned int *)DDR_START_ADDR, 0);		// ��ȡSD����DDR��
	led1();
	delay();
	// �ڶ�������ת��DDR�е�BL2ȥִ��
	pBL2Type p2 = (pBL2Type)DDR_START_ADDR;
	p2();
	
	led3();
	delay();
}






















