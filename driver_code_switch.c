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

static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class
int *log_addr;

static int my_open(struct inode *i, struct file *f)
{
	log_addr = ioremap(0x840040, 4096);	/* 0x840040 is the address of GPIO_48 pin */
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	iounmap(log_addr);
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t count, loff_t *f_pos)
{
	printk(KERN_INFO "Driver: read()\n");
	char buffer[10];

	if (gpio_direction_input(GPIO_NUMBER) < 0)
		printk(KERN_ALERT "Setting gpio pin as input\n");

	int temp = gpio_get_value(GPIO_NUMBER);

	sprintf( buffer, "%1d" , temp );

	count = sizeof( buffer );

	if( copy_to_user( buf, buffer, count ) )
		return -EFAULT;

	if( *f_pos == 0 )
	{
		*f_pos += 1;
		return 1;
	}
	else
	{
		return 0;
	}
	return 0;
}

/*
static ssize_t my_write(struct file *f, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk(KERN_INFO "Driver: write()\n");
	printk(KERN_INFO "Executing WRITE.\n");

	switch( buf[0] )
	{
		case '0':
			if (gpio_direction_output(GPIO_NUMBER, 0) < 0)
				printk(KERN_ALERT "Setting gpio pin as output\n");
			gpio_set_value(GPIO_NUMBER, 0);
			break;

		case '1':
			if (gpio_direction_output(GPIO_NUMBER, 1) < 0)
				printk(KERN_ALERT "Setting gpio pin as output\n");
			gpio_set_value(GPIO_NUMBER, 1);
			break;

		default:
			printk("Wrong option.\n");
			break;
	}
	return count;
}
*/

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
	if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "gpio")))
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
MODULE_AUTHOR("Anil Kumar Pugalia <email@sarika-pugs.com>");
MODULE_DESCRIPTION("Our First Character Driver");
