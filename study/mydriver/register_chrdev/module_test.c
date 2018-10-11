#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>

#define MYMAJOR		200
#define MYNAME		"testchar"


static int test_chrdev_open(struct inode *inode, struct file *file)
{
	// 这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
	// 但是现在暂时我们写不了这么多，所以用一个printk打印个信息来做代表。
	printk(KERN_INFO "test_chrdev_open\n");
	
	return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "test_chrdev_release\n");
	
	return 0;
}

// 自定义一个file_operations结构体变量，并且去填充
static const struct file_operations test_fops = {
	.owner		= THIS_MODULE,				// 惯例，直接写即可
	
	.open		= test_chrdev_open,			// 将来应用open打开这个设备时实际调用的
	.release	= test_chrdev_release,		// 就是这个.open对应的函数
};


// 模块安装函数
static int __init chrdev_init(void)
{	
	int ret = -1;
	
	printk(KERN_INFO "chrdev_init helloworld init\n");

	// 在module_init宏调用的函数中去注册字符设备驱动
		   // see from : cat /proc/devices
	ret = register_chrdev(MYMAJOR, MYNAME, &test_fops);

	if (ret)
	{
		printk(KERN_ERR "register_chrdev fail\n");
		return -EINVAL;
	}
	printk(KERN_INFO "register_chrdev success...\n");

	return 0;
}

// 模块下载函数
static void __exit chrdev_exit(void)
{
	printk(KERN_INFO "chrdev_exit helloworld exit\n");
	
	// 在module_exit宏调用的函数中去注销字符设备驱动
	unregister_chrdev(MYMAJOR, MYNAME);
	
}


module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("jimmy");				// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息








