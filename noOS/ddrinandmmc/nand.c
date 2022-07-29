#include "cpu_base.h"
#include "nand.h"
#include "debug.h"
#include "stdio.h"
#define TACLS    					  1
#define TWRPH0   					  1
#define TWRPH1   					  1

//#define NFCONF_VALUE    ((TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(0x1<<1))
//#define NFCONT_VALUE    ((0x1<<23)|(0x1<<22)|(0x2<<1)|(0x1<<0))

#define NFCONF_VALUE  (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(0<<3)|(0<<2)|(1<<1)|(0<<0) 
#define NFCONT_VALUE  (0<<18)|(0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(0<<7)|(0<<6)|(0x3<<1)|(1<<0) 
static struct nand_chip mynand;
/*void nand_init(void) 
{ 
// 1. 配置NAND Flash 
NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(0<<3)|(0<<2)|(1<<1)|(0<<0); 
NFCONT =(0<<18)|(0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(0<<7)|(0<<6)|(0x3<<1)|(1<<0); 
// 2. 配置引脚 
MP0_1CON = 0x22333322; 
MP0_2CON = 0x00002222; 
MP0_3CON = 0x22222222; 
// 3. 复位 
//nand_reset();
}*/
/*
static void nand_reset(void) 
{ 
    nand_select_chip();
    nand_send_cmd(NAND_CMD_RES); 
    nand_wait_idle(); 
    nand_deselect_chip(); 
} */
void nand_lowlevel_init()
{
    unsigned int var;

    	printf("iinside nand low level \n");
    /* Setup NFCSn0 to NFCE */
    var = readl(S5PV210_MP0_1CON);
    var &= ~(0xf<<8);
    var |= (0x3<<8);
  //  var=0x22333322;
    writel(var,S5PV210_MP0_1CON);

//    __REG(S5PV210_MP0_2CON) = 0x00002222; 
    /* Setup Nand Control pin Function to CLE - ALE - RE - WE - RnB[0-3]*/
    __REG(S5PV210_MP0_3CON) = 0x22222222; 

    /* Setup Nand Control init data */
    writel(NFCONF_VALUE,NFCONF);
    writel(NFCONT_VALUE,NFCONT);
//nand_init();
    /* Pass NFCON Gating Clock */
    //var = readl(CLK_GATE_IP1);
    //var |= ((0xf1<<24)|(0x3<<16)|(0xf<<8)|(0x7<<0));
    //writel(var,CLK_GATE_IP1);
//NFCONT &= ~(1<<1);

}

static void s5p_nand_hwcontrol(int cmd, unsigned int ctrl)
{
    unsigned int cur;
	
    if(ctrl & NAND_CTRL_CHANGE){
        if(ctrl & NAND_NCE){
            if(cmd != NAND_CMD_NONE) {
                cur = readl(NFCONT);
                cur &= ~S5P_NFCONT_nFCE0;
                writel(cur, NFCONT);
            }
        }else {
            cur = readl(NFCONT);
            cur |= S5P_NFCONT_nFCE0;
            writel(cur, NFCONT);
        }
    }

    if(cmd != NAND_CMD_NONE){
		if (ctrl & NAND_CLE)
			writeb(cmd, NFCMMD);
		else if (ctrl & NAND_ALE)
			writeb(cmd, NFADDR);
	}
	
}

static int s5p_nand_device_ready(void)
{
    unsigned int var;

    var = readl(NFSTAT);
	return ( var & S5P_NFSTAT_READY);
}

static unsigned char s5p_nand_read_byte(void)
{
    return readb(NFDATA);
}

static void s5p_nand_select_chip(int chipnr)
{
    struct nand_chip *chip = &mynand;

	switch(chipnr){
	    case -1:
		    chip->cmd_ctrl(NAND_CMD_NONE, 0 | NAND_CTRL_CHANGE);
		    break;
	    case 0:
		    break;
	}
}

static void nand_wait_ready()
{
	struct nand_chip *chip = &mynand;

	while(1){
		if(chip->dev_ready())
			break;
	}
}

static void s5p_nand_lpcommand(unsigned int command,int column,int page_addr)
{
    struct nand_chip *chip = &mynand;

	/* Emulate NAND_CMD_READOOB */

	/* Command latch cycle */
	chip->cmd_ctrl(command, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);

	if(column != -1 || page_addr != -1){
		int ctrl = NAND_CTRL_CHANGE | NAND_NCE | NAND_ALE;
		/* Serially input address */
		if (column != -1) {
			chip->cmd_ctrl(column, ctrl);
			ctrl &= ~NAND_CTRL_CHANGE;
			chip->cmd_ctrl(column >> 8, ctrl);
		}
		if (page_addr != -1) {
			chip->cmd_ctrl(page_addr, ctrl);
			chip->cmd_ctrl(page_addr >> 8, NAND_NCE | NAND_ALE);
			/* One more address cycle for devices > 128MiB */
			chip->cmd_ctrl(page_addr >> 16,NAND_NCE | NAND_ALE);
		}
	}
	chip->cmd_ctrl(NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

	/*
	 * program and erase have their own busy handlers
	 * status, sequential in, and deplete1 need no delay
	 */
	switch(command){
    	case NAND_CMD_CACHEDPROG:
    	case NAND_CMD_PAGEPROG:
    	case NAND_CMD_ERASE1:
    	case NAND_CMD_ERASE2:
    	case NAND_CMD_SEQIN:
    	case NAND_CMD_RNDIN:
    	case NAND_CMD_STATUS:
    		return;

	    case NAND_CMD_RESET:
	        if(chip->dev_ready){
			    break;
		    }
		    chip->cmd_ctrl(NAND_CMD_STATUS,NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		    chip->cmd_ctrl(NAND_CMD_NONE,NAND_NCE | NAND_CTRL_CHANGE);
		    while (!(chip->read_byte() & NAND_STATUS_READY))
				;
	        return;

	    case NAND_CMD_RNDOUT:
		    /* No ready / busy check necessary */
		    chip->cmd_ctrl(NAND_CMD_RNDOUTSTART,NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		    chip->cmd_ctrl(NAND_CMD_NONE,NAND_NCE | NAND_CTRL_CHANGE);
		    return;

	    case NAND_CMD_READ0:
		    chip->cmd_ctrl(NAND_CMD_READSTART,NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		    chip->cmd_ctrl(NAND_CMD_NONE,NAND_NCE | NAND_CTRL_CHANGE);

		/* This applies to read commands */
	    default:
	        /*
		        * If we don't have access to the busy pin, we apply the given
		        * command delay
		        */
	        if (!chip->dev_ready){
	            //udelay();
			    return;
		    }
	}

	nand_wait_ready();
}

static void read_chip_id()
{
    struct nand_chip *nand = &mynand;
    unsigned char chip_id[6];
    int i;

    nand->cmd_ctrl(NAND_CMD_READID,NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
    nand->cmd_ctrl(0x00,NAND_NCE | NAND_ALE | NAND_CTRL_CHANGE);

    while( !(nand->dev_ready()));
    for(i = 0; i < 6; i++){
        chip_id[i] = nand->read_byte();
        printf("the chipid[%d] is %x\n",i,chip_id[i]);
    }
}

void nand_init_chip(void)
{
    struct nand_chip *nand = &mynand;

    nand->cmd_ctrl		= s5p_nand_hwcontrol;
	nand->dev_ready		= s5p_nand_device_ready;

	nand->read_byte     = s5p_nand_read_byte;
	nand->select_chip   = s5p_nand_select_chip;
	nand->cmdfunc       = s5p_nand_lpcommand;

    nand->cmd_ctrl(NAND_CMD_NONE,NAND_NCE | NAND_CTRL_CHANGE);
    nand->cmdfunc(NAND_CMD_RESET, -1, -1);

	read_chip_id();
	nand->cmdfunc(NAND_CMD_RESET, -1, -1);

	return ;
}

#define PAGE_SIZE 2048
int nand_read(void *data,unsigned int start_addr,unsigned int size)
{
    struct nand_chip *nand = &mynand;
    unsigned char *rd = data;
    unsigned int colum;
    unsigned int page;
    int i;

    if(rd == 0x0){
        return -1;
    }
    colum = start_addr % PAGE_SIZE;
    page = start_addr / PAGE_SIZE;

    nand->cmdfunc(NAND_CMD_READ0,colum,page);

    for(i = 0; i < size; i++){
        rd[i] = nand->read_byte();
	printf("rd[%d]=%d\n",i,rd[i]);
    }

    return i;
}

static int nand_read_status()
{
    struct nand_chip *nand = &mynand;
    unsigned char status;

    nand->cmd_ctrl(NAND_CMD_STATUS,NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);

    nand_wait_ready();
    status = nand->read_byte();
    return status;
}

int erase_flash(unsigned int start_addr)
{
    struct nand_chip *nand = &mynand;
    int page = start_addr /PAGE_SIZE;

    nand->cmdfunc(NAND_CMD_ERASE1,-1,page);
    nand->cmdfunc(NAND_CMD_ERASE2,-1,-1);

    if (nand_read_status() & 0x1){
        printf("erase %x block failed!\n",page);
        return -1;
    }
    else{
        printf("erase %x block success!\n",page);
        return 0;
    }
}

int nand_write(const void *data,unsigned int start_addr,unsigned int size)
{
    struct nand_chip *nand = &mynand;
    const unsigned char *wd = data;
    unsigned int colum;
    unsigned int page;
    int i;

    if(wd == 0x0){
        return -1;
    }
    colum = start_addr % PAGE_SIZE;
    page = start_addr / PAGE_SIZE;

    printf("the colum is %x,the page is %x\n",colum,page);

    /*The page must be 0x00 */
    nand->cmdfunc(NAND_CMD_SEQIN,colum,page);
    for(i = 0; i < size; i++){
        writeb(wd[i],NFDATA);
    }
    nand->cmdfunc(NAND_CMD_PAGEPROG, -1, -1);

    if (nand_read_status() & 0x1){
        printf("write %x failed!\n",start_addr);
        return -1;
    }
    else{
        printf("write %x success!\n",start_addr);
        return 0;
    }
}

