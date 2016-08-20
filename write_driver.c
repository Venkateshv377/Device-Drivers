#include <linux/kernel.h>
#include <linux/kern_levels.h>
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

#define GPIO_NUMBER		48
#define GPIO_ADDR		0x4804C000


static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class
unsigned int gpio_logical_addr;

static int my_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: open()\n");

	gpio_logical_addr = ioremap(GPIO_ADDR, 0x400);	/* 0x4804C000 is the address of GPIO_48 pin */

	iowrite32(~(1 << 16), gpio_logical_addr + 0x134);

	printk(KERN_ALERT "GPIO_ADDR is %d\n", gpio_logical_addr);
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
	printk(KERN_INFO "Driver: read()\n");
	return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk(KERN_INFO "Driver: write()\n");
	printk(KERN_INFO "Executing WRITE.\n");

	switch( buf[0] )
	{
		case '0':
				iowrite32(1 << 16, gpio_logical_addr + 0x190);
				break;
		case '1':
				iowrite32(1 << 16, gpio_logical_addr + 0x194);
				break;
	}
	return count;
}

static struct file_operations pugs_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
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
	if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "gpio_write")))
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
