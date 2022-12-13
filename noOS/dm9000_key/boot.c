#include "atag.h"
#include "string.h"

#define SDRAM_KERNEL_START 0x20008000
#define SDRAM_TAGS_START   0x20000100
#define SDRAM_ADDR_START   0x20000000
#define SDRAM_TOTAL_SIZE   0x20000000


void (*theKernel)(int , int , unsigned int );

struct tag *pCurTag;

const char *cmdline = "console=ttySAC2,115200";


void setup_core_tag()
{
     pCurTag = (struct tag *)SDRAM_TAGS_START;
     
     pCurTag->hdr.tag = ATAG_CORE;
     pCurTag->hdr.size = tag_size(tag_core); 
     
     pCurTag->u.core.flags = 0;
     pCurTag->u.core.pagesize = 4096;
     pCurTag->u.core.rootdev = 0;
     
     pCurTag = tag_next(pCurTag);
     printf("core args done!\r\n");
}

void setup_mem_tag()
{
     pCurTag->hdr.tag = ATAG_MEM;
     pCurTag->hdr.size = tag_size(tag_mem32); 
     
     pCurTag->u.mem.start = SDRAM_ADDR_START;
     pCurTag->u.mem.size = SDRAM_TOTAL_SIZE;
     
     pCurTag = tag_next(pCurTag);
     printf("mem args done!\r\n");
}

void setup_cmdline_tag()
{
     int linelen = strlen(cmdline);
     
     pCurTag->hdr.tag = ATAG_CMDLINE;
     pCurTag->hdr.size = (sizeof(struct tag_header)+linelen+1+4)>>2;
     
     strcpy(pCurTag->u.cmdline.cmdline,cmdline);
     
     pCurTag = tag_next(pCurTag);
     printf("cmd args done!\r\n");
}

void setup_end_tag()
{
	pCurTag->hdr.tag = ATAG_NONE;
	pCurTag->hdr.size = 0;
	printf("end args!\r\n");
}



void boot_linux()
{
    //1. 获取Linux启动地址
    theKernel = (void (*)(int , int , unsigned int ))SDRAM_KERNEL_START;
    
    //2. 设置启动参数
    //2.1 设置核心启动参数
    setup_core_tag();
    
    //2.2 设置内存参数
    setup_mem_tag();
    
    //2.3 设置命令行参数
    setup_cmdline_tag();
    
    //2.4 设置参数结束标志
    setup_end_tag();
    
    printf("booting the kernel!\r\n");
    //3. 启动Linux系统
    theKernel(0,2456,SDRAM_TAGS_START);
}