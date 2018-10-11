#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>
#include <linux/io.h>
#include <linux/ioport.h>


#define MYMAJOR 	250
#define NAME	"test"

#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

#define rGPJ0CON	*((volatile unsigned int *)GPJ0CON)
#define rGPJ0DAT	*((volatile unsigned int *)GPJ0DAT)

#define GPJ0CON_PA	0xE0200240
#define GPJ0DAT_PA	0xE0200244

volatile unsigned int *pGPJ0CON;
volatile unsigned int *pGPJ0DAT;



int major;

char kbuf[3];


static int test_chrdev_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "test_chrdev_open\n");

	
	return 0;
}


static int test_chrdev_close(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "test_chrdev_close\n");
	
	
	
	
	return 0;
}

static ssize_t test_chrdev_write(struct file *file,
		const char __user *buffer, size_t count, loff_t *pos)
{
	int ret = -1;
	
	printk(KERN_INFO "test_chrdev_write\n");
	memset(kbuf, 0, 100);
	ret = copy_from_user(kbuf, buffer, 7);
	if (ret)
	{
		printk(KERN_ERR "copy_from_user error, ret = %d.\n", ret);
		return ret;
	}
	printk(KERN_INFO "copy_from_user ok, ret = %d.\n", ret);
	

	return 0;
}

ssize_t test_chrdev_read(struct file *file, 
		char __user *buf, size_t size, loff_t *ppos)
{
	int ret = -1;
	
	printk(KERN_INFO "test_chrdev_read\n");
	ret = copy_to_user(buf, kbuf, strlen(kbuf));
	if (ret)
	{
		printk(KERN_ERR "copy_to_user error, ret = %d.\n", ret);
		return ret;
	}
	printk(KERN_INFO "copy_to_user ok, ret = %d.\n", ret);

	return 0;	
}

static const struct file_operations test_fops = {
	.owner		= THIS_MODULE,
	.open		= test_chrdev_open,
	.release	= test_chrdev_close,
	.write 		= test_chrdev_write,
	.read		= test_chrdev_read,
};


// 模块安装函数
static int __init chrdev_init(void)
{	
	int ret = -1;

	printk(KERN_INFO "chrdev_init helloworld init\n");

	major = register_chrdev(0, NAME, &test_fops);
	
	printk(KERN_INFO "major = %d\n", major);
	printk(KERN_INFO "register_chrdev success\n");
	
/*
	rGPJ0CON = 0x11111111;
	rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮
	
	printk(KERN_INFO "GPJ0CON = %p\n", GPJ0CON);
*/

	if (!request_mem_region(GPJ0CON_PA, 4, "GPJ0CON"))
		return -EINVAL;
	if (!request_mem_region(GPJ0DAT_PA, 4, "GPJ0DAT"))
		return -EINVAL;
	
	pGPJ0CON = ioremap(GPJ0CON_PA, 4);
	pGPJ0DAT = ioremap(GPJ0DAT_PA, 4);
	
	*pGPJ0CON = 0x11111111;
	*pGPJ0DAT = rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));	
	
	printk(KERN_INFO "GPJ0CON = %p.\n", pGPJ0CON);
	printk(KERN_INFO "GPJ0DAT = %p.\n", pGPJ0DAT);
	



	return 0;
}

// 模块下载函数
static void __exit chrdev_exit(void)
{
	unregister_chrdev(major, NAME);
	
	//rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
	*pGPJ0DAT = rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
	
	printk(KERN_INFO "chrdev_exit helloworld exit\n");
}


module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("aston");				// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息








