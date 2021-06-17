#include "int.h"
#include "stdio.h"


void reset_exception(void)
{
	printf("reset_exception.\n");
}

void undef_exception(void)
{
	printf("undef_exception.\n");
}

void sotf_int_exception(void)
{
	printf("sotf_int_exception.\n");
}

void prefetch_exception(void)
{
	printf("prefetch_exception.\n");
}

void data_exception(void)
{
	printf("data_exception.\n");
}

// ��Ҫ���ܣ��󶨵�һ�׶��쳣��������ֹ�����жϣ�ѡ�������ж�����ΪIRQ��
// ���VICnADDRΪ0
void system_init_exception(void)
{
	// ��һ�׶δ������쳣������
	r_exception_reset = (unsigned int)reset_exception;
	r_exception_undef = (unsigned int)undef_exception;
	r_exception_sotf_int = (unsigned int)sotf_int_exception;
	r_exception_prefetch = (unsigned int)prefetch_exception;
	r_exception_data = (unsigned int)data_exception;
	r_exception_irq = (unsigned int)IRQ_handle;
	r_exception_fiq = (unsigned int)IRQ_handle;
	
	// ��ʼ���жϿ������Ļ����Ĵ���
	intc_init();
}

// �����Ҫ������жϵ��жϴ������ĵ�ַ
void intc_clearvectaddr(void)
{
    // VICxADDR:��ǰ���ڴ�����жϵ��жϴ������ĵ�ַ
    VIC0ADDR = 0;
    VIC1ADDR = 0;
    VIC2ADDR = 0;
    VIC3ADDR = 0;
}

// ��ʼ���жϿ�����
void intc_init(void)
{
    // ��ֹ�����ж�
	// Ϊʲô���жϳ�ʼ��֮��Ҫ��ֹ�����жϣ�
	// ��Ϊ�ж�һ���򿪣���Ϊ�ⲿ����Ӳ���Լ���ԭ������жϺ�һ���ͻ�Ѱ��isr
	// �����ǿ�����Ϊ�Լ��ò�������жϾ�û���ṩisr����ʱ���Զ��õ��ľ�������
	// �����ܿ����ܷɣ����Բ��õ��ж�һ��Ҫ�ص���
	// һ�����������ȫ���ص���Ȼ������һ���Լ�����Ȥ���жϡ�һ���򿪾ͱ���
	// ������ж��ṩ��Ӧ��isr���󶨺á�
    VIC0INTENCLEAR = 0xffffffff;
    VIC1INTENCLEAR = 0xffffffff;
    VIC2INTENCLEAR = 0xffffffff;
    VIC3INTENCLEAR = 0xffffffff;

    // ѡ���ж�����ΪIRQ
    VIC0INTSELECT = 0x0;
    VIC1INTSELECT = 0x0;
    VIC2INTSELECT = 0x0;
    VIC3INTSELECT = 0x0;

    // ��VICxADDR
    intc_clearvectaddr();
}


// ������д��isr��VICnVECTADDR�Ĵ���
// �󶨹�֮�����ǾͰ�isr��ַ����Ӳ���ˣ�ʣ�µ����ǲ��ù��ˣ�Ӳ���Լ��ᴦ��
// �ȷ�����Ӧ�жϵ�ʱ������ֱ�ӵ���Ӧ��VICnADDR��ȥȡisr��ַ���ɡ�
// ������intnum��int.h����������жϺţ�handler�Ǻ���ָ�룬��������д��isr

// VIC0VECTADDR����ΪVIC0VECTADDR0�Ĵ����ĵ�ַ�����൱����VIC0VECTADDR0��31���
// ���飨����������һ������ָ�����飩���׵�ַ��Ȼ��������ÿһ���жϵ�ʱ��
// ֻ��Ҫ�׵�ַ+ƫ�������ɡ�
void intc_setvectaddr(unsigned long intnum, void (*handler)(void))
{
    //VIC0
    if(intnum<32)
    {
        *( (volatile unsigned long *)(VIC0VECTADDR + 4*(intnum-0)) ) = (unsigned)handler;
    }
    //VIC1
    else if(intnum<64)
    {
        *( (volatile unsigned long *)(VIC1VECTADDR + 4*(intnum-32)) ) = (unsigned)handler;
    }
    //VIC2
    else if(intnum<96)
    {
        *( (volatile unsigned long *)(VIC2VECTADDR + 4*(intnum-64)) ) = (unsigned)handler;
    }
    //VIC3
    else
    {
        *( (volatile unsigned long *)(VIC3VECTADDR + 4*(intnum-96)) ) = (unsigned)handler;
    }
    return;
}


// ʹ���ж�
// ͨ�����ε�intnum��ʹ��ĳ��������ж�Դ���жϺ���int.h�ж��壬�������жϺ�
void intc_enable(unsigned long intnum)
{
    unsigned long temp;
	// ȷ��intnum���ĸ��Ĵ�������һλ
	// <32����0��31����Ȼ��VIC0
    if(intnum<32)
    {
        temp = VIC0INTENABLE;
        temp |= (1<<intnum);		// ����ǵ�һ����������λ�������ڶ�����ƿ���
									// ֱ��д��
        VIC0INTENABLE = temp;
    }
    else if(intnum<64)
    {
        temp = VIC1INTENABLE;
        temp |= (1<<(intnum-32));
        VIC1INTENABLE = temp;
    }
    else if(intnum<96)
    {
        temp = VIC2INTENABLE;
        temp |= (1<<(intnum-64));
        VIC2INTENABLE = temp;
    }
    else if(intnum<NUM_ALL)
    {
        temp = VIC3INTENABLE;
        temp |= (1<<(intnum-96));
        VIC3INTENABLE = temp;
    }
    // NUM_ALL : enable all interrupt
    else
    {
        VIC0INTENABLE = 0xFFFFFFFF;
        VIC1INTENABLE = 0xFFFFFFFF;
        VIC2INTENABLE = 0xFFFFFFFF;
        VIC3INTENABLE = 0xFFFFFFFF;
    }

}

// ��ֹ�ж�
// ͨ�����ε�intnum����ֹĳ��������ж�Դ���жϺ���int.h�ж��壬�������жϺ�
void intc_disable(unsigned long intnum)
{
    unsigned long temp;

    if(intnum<32)
    {
        temp = VIC0INTENCLEAR;
        temp |= (1<<intnum);
        VIC0INTENCLEAR = temp;
    }
    else if(intnum<64)
    {
        temp = VIC1INTENCLEAR;
        temp |= (1<<(intnum-32));
        VIC1INTENCLEAR = temp;
    }
    else if(intnum<96)
    {
        temp = VIC2INTENCLEAR;
        temp |= (1<<(intnum-64));
        VIC2INTENCLEAR = temp;
    }
    else if(intnum<NUM_ALL)
    {
        temp = VIC3INTENCLEAR;
        temp |= (1<<(intnum-96));
        VIC3INTENCLEAR = temp;
    }
    // NUM_ALL : disable all interrupt
    else
    {
        VIC0INTENCLEAR = 0xFFFFFFFF;
        VIC1INTENCLEAR = 0xFFFFFFFF;
        VIC2INTENCLEAR = 0xFFFFFFFF;
        VIC3INTENCLEAR = 0xFFFFFFFF;
    }

    return;
}


// ͨ����ȡVICnIRQSTATUS�Ĵ������ж������ĸ���һλΪ1������֪�ĸ�VIC�����ж���
unsigned long intc_getvicirqstatus(unsigned long ucontroller)
{
    if(ucontroller == 0)
        return	VIC0IRQSTATUS;
    else if(ucontroller == 1)
        return 	VIC1IRQSTATUS;
    else if(ucontroller == 2)
        return 	VIC2IRQSTATUS;
    else if(ucontroller == 3)
        return 	VIC3IRQSTATUS;
    else
    {}
    return 0;
}


// �������жϴ��������˼����˵����ֻ�����жϴ��������Ǳ���/�ָ��ֳ�
void irq_handler(void)
{
	printf("irq_handler.\n");
	// SoC֧�ֺܶ�����ڵͶ�CPU����2440����30�������210����100������ж�
	// ��ô���ж�irq�ڵ�һ���׶��ߵ���һ��·��������뵽irq_handler��
	// ������irq_handler��Ҫȥ���־������ĸ��жϷ����ˣ�Ȼ����ȥ���ø��ж�
	// ��Ӧ��isr��
	
	
	// ��ȻӲ���Ѿ��Զ������ǰ�isr������VICnADDR�У�������Ϊ��4�����������Ǳ���
	// ��ȥ����ļ����������ĸ�VIC�ж��ˣ�Ҳ����˵isr�������ĸ�VICADDR�Ĵ�����
	unsigned long vicaddr[4] = {VIC0ADDR,VIC1ADDR,VIC2ADDR,VIC3ADDR};
    int i=0;
    void (*isr)(void) = NULL;

    for(i=0; i<4; i++)
    {
		// ����һ���ж�ʱ��4��VIC����3����ȫ0��1��������һλ����0
        if(intc_getvicirqstatus(i) != 0)
        {
            isr = (void (*)(void)) vicaddr[i];
            break;
        }
    }
    (*isr)();		// ͨ������ָ�������ú���
	//isr();			// ��ȷ�ģ��������һ��Ч��
	//*isr();		// ����
}







