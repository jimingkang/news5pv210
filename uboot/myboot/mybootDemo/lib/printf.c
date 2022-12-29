#include "vsprintf.h"

unsigned char outbuf[1024];
unsigned char inbuf[1024];

int printf(const char* fmt, ...)
{
    va_list args;
    int i;
	
    //1.将变参转化为字符串
    /*将可变参数的首地址，赋值给args,va_list为指向char类型的指针*/
    va_start(args,fmt);
    
    /**
    *内部调用va_arg宏
    *va_arg宏的作用，取出args指向的可变参数，并指向下一个可变参数
    */
    vsprintf((char *)outbuf, fmt, args);
    
    va_end(args);
    
    //2. 打印字符串到串口
    for(i=0;i< strlen((char *)outbuf); i++)
    {
        uart0_sendbyte(outbuf[i]);
        lcd_draw_char(outbuf[i]);	
    }
    
    return i;
}

int scanf(const char* fmt, ...)
{
	unsigned char c;
	int i = 0;
	va_list args;
	
	//1. 获取输入的字符串
	
	while (1)
	{
		
	    c = uart0_getbyte();	
	    if ((c==0x0d) || (c==0x0a))
	    {
	    	inbuf[i] = '\n';
	    	break;
	    }
	    else
	    {
	        inbuf[i++] = c;	
	    }
	}
	
	//2. 格式转化
	va_start(args, fmt);
	vsscanf((char *)inbuf,fmt,args);
	va_end(args);
	
	return i;
}
