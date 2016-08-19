#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/io.h>

#define GPIO_NUMBER	48
#define GPIO_ADDR	0x4804C000

static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class
unsigned int gpio_logical_addr;

static int my_open(struct inode *i, struct file *f)
{
	gpio_logical_addr = ioremap(GPIO_ADDR, 0x400);	/* 0x840040 is the address of GPIO_48 pin */
	iowrite32(~(0 << 16), gpio_logical_addr + 0x134);
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	iounmap(gpio_logical_addr);
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t count, loff_t *f_pos)
{
	char str;
	unsigned int gpio_datain = (u32)(ioread32(gpio_logical_addr + 0x138)); 
	printk(KERN_INFO "GPIO Val = %x\n", gpio_datain);
	printk(KERN_INFO "Driver: read()\n");

	if (gpio_datain & (1 << 16))
	{
		printk(KERN_INFO "Switch is pressed\n");
		str = '1';
		if (*f_pos != 0)
			return 0;
	}
	else
	{
		printk(KERN_INFO "The switch is not pressed\n");
		str = '0';
		if( *f_pos != 0 )
			return 0;
	}
	str = '\0';

	if(copy_to_user(buf, str, 1) != 0)
		return EFAULT;
	
	*f_pos = str;
	
	return &str;
}

static struct file_operations pugs_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	//	.write = my_write
};

static int __init ofcd_init(void) /* Constructor */
{
	int ret;
	struct device *dev_ret;

	printk(KERN_INFO "Namaskar: ofcd registered");
	if ((ret = alloc_chrdev_region(&first, 0, 1, "GPIO")) < 0)
	{
		return ret;
	}
	if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv")))
	{
		unregister_chrdev_region(first, 1);
		return PTR_ERR(cl);
	}
	/*Creating the device in the /dev entry */
	if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "gpio_read")))
	{
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return PTR_ERR(dev_ret);
	}

	/* Adding the device into the /dev entry. */
	cdev_init(&c_dev, &pugs_fops);
	if ((ret = cdev_add(&c_dev, first, 1)) < 0)
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return ret;
	}
	return 0;
}

static void __exit ofcd_exit(void) /* Destructor */
{
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "Alvida: ofcd unregistered");
}

module_init(ofcd_init);
module_exit(ofcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Venkatesh <email: venkateshv377@gmail.com>");
MODULE_DESCRIPTION("My First Character Driver");
