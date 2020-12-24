#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>

#include <asm/uaccess.h>
#include <linux/fs.h>
struct mydev_st{
	char buf[256];
	dev_t  no;
	struct cdev dev;
	int count;
};
struct mydev_st *mydev;
int chrdev_open(struct inode *node,struct file *fp){
struct mydev_st *m;
m=container_of(node->i_cdev,struct mydev_st,dev);
memset(m->buf,0 ,256);
m->count++;
fp->private_data=m;
return 0;

}
ssize_t chrdev_read(struct file *fp,char __user * buf,size_t count,loff_t * off){
struct mydev_st *m;
int ret;
m=fp->private_data;
count=min((int)count,256);
ret=copy_to_user(buf,m->buf,count);
return ret;
}

ssize_t chrdev_write(struct file *fp,const char __user  * buf,size_t count,loff_t * off){
struct mydev_st *m;
int ret;
m=fp->private_data;
count=min((int)count,256);
ret=copy_from_user(m->buf,buf,count);
if(ret<0){
ret= -EFAULT;
goto copy_error;

}
return count;
copy_error:

return ret;
}
ssize_t chrdev_close(struct inode *inode,struct file *fp){
return 0;
}
static const struct file_operations my_fops = {
    .owner        = THIS_MODULE,           // 惯例，直接写即可
    .open        = chrdev_open,          // 将来应用open打开这个设备时实际调用的
    .release    = chrdev_close,        // 就是这个.open对应的函数
    .write       = chrdev_write,
    .read        =chrdev_read,
};
// 模块安装函数
static int __init chrdev_init(void)
{	int ret;
	mydev=kzalloc(sizeof(*mydev),GFP_KERNEL);
	
	if(!mydev){
	ret=-ENOMEM;
	goto alloc_dev_error;
	}
	ret=alloc_chrdev_region(&mydev->no,1,1,"mydev");
	if(ret<0){
	goto alloc_no_error;
	}
	cdev_init(&mydev->dev,&my_fops);
	ret=cdev_add(&mydev->dev,mydev->no,1);
	if(ret<0)
	{
		goto cdev_add_error;
	}
	printk(KERN_INFO "chrdev_init major=%d minor=%d\n",MAJOR(mydev->no),MINOR(mydev->no));
	return 0;
cdev_add_error:
	unregister_chrdev_region(mydev->no,1);
alloc_no_error:
	kfree(mydev);
alloc_dev_error:
         	
	return ret;
}

// 模块下载函数
static void __exit chrdev_exit(void)
{
	printk(KERN_INFO "chrdev_exit \n");
	cdev_del(&mydev->dev);
	unregister_chrdev_region(mydev->no,1);
	kfree(mydev);
}


module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("jimmy");				// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息








