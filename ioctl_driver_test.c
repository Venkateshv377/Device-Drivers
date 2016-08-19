#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define QUERY_GET_VALUE _IOR('q', 1, int *)
#define QUERY_CLEAR_VALUE _IO('q', 2)
#define QUERY_SET_VALUE _IOW('q', 3, int *)

MODULE_LICENSE("GPL");

static char ker_buf[100];   //driver local  buffer

static int dev_open(struct inode *inod, struct file *fil);

static ssize_t dev_read(struct file *filep,char *buf,size_t len,loff_t *off);

static ssize_t dev_write(struct file *flip,const char *buff,size_t len,loff_t *off);

static int dev_release(struct inode *inod,struct file *fil);

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	int a;
	static int i;

	switch(cmd)
	{
		case QUERY_GET_VALUE:
			copy_to_user((int*)arg,&a,sizeof(int));
			break;

		case QUERY_SET_VALUE:
			copy_from_user(&a,(int *)arg,sizeof(int));
			printk(KERN_ALERT "a=%d\t &a=%p\n", a, &a);
			if (a == 1)
			{
				for (i = 0; i < sizeof(ker_buf); i++)
				{
					if (ker_buf[i] >= 97 && ker_buf[i] <= 122)
					{
						ker_buf[i] = ker_buf[i] - 32;
					}
				}
			}
			else if ( a == 2)
			{
				for (i = 0; i < sizeof(ker_buf); i++)
				{
					if (ker_buf[i] =='\0')
					{
						break;
					}
					ker_buf[i] = ker_buf[i] + 1;
				}
			}
			break;

		case QUERY_CLEAR_VALUE:
			break;
	}
	return 0;
}

//structure containing device operation

static struct file_operations fops=

{
	.owner = THIS_MODULE,		 

	.open=dev_open,   //pointer to device open function
	
	.read=dev_read,  //pointer to device read funtion

	.write=dev_write, //pointer to device write function

	.release=dev_release, //pointer to device realese function

	.unlocked_ioctl=my_ioctl, //ioctl added by me
};

static int hello_init(void)   //init function to be called at the time of insmod

{

	int t=register_chrdev(95,"mydev",&fops);

	if(t<0)

		printk(KERN_ALERT "device registration failed\n");

	else

		printk(KERN_ALERT "device registred\n");

	return 0;

}

static void hello_exit(void) //exit function to be called at the time of rmmod
{
	unregister_chrdev(95,"mydev");
	printk(KERN_ALERT "exit");
}

static int dev_open(struct inode *inod, struct file *fil)
{
	printk(KERN_ALERT "device opened\n");
	return 0;
}

static ssize_t dev_read(struct file *filep,char *buf,size_t len,loff_t *off)
{
	printk(KERN_ALERT "device readed\n");
	copy_to_user(buf,ker_buf,len);
	return len;
}

static ssize_t dev_write(struct file *flip,const char *buf,size_t len,loff_t *off)
{
	printk(KERN_ALERT "device writted\n");

	printk(KERN_ALERT " %s\n", buf);

	copy_from_user(ker_buf,buf,len);
	printk(KERN_ALERT " %s\n", ker_buf);
//	ker_buf[len]=0;	

	return len;
}

static int dev_release(struct inode *inod,struct file *fil)
{
	printk(KERN_ALERT "device closed\n");
	return 0;
}

module_init(hello_init);

module_exit(hello_exit);

