#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h> 

#define GPIO 48
int ex07_open(struct inode *pinode, struct file *pfile)
{
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	ioremap(0x44E07000, 4096);
	return 0;
}

ssize_t ex07_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	return 0;
}

ssize_t ex07_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	return length;
}

int ex07_close(struct inode *pinode, struct file *pfile)
{
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	iounmap(0x44E07000);
	return 0;
}
/* To hold the file operations performed on this device */
struct file_operations ex07_file_operations = { 
	.owner          = THIS_MODULE,
	.open           = ex07_open,
	.read           = ex07_read,
	.write          = ex07_write,
	.release        = ex07_close,
};


static irqreturn_t irq_handler(int irq_line, void *dev_id)
{
	printk("Inside the interrupt handler\n");
	return IRQ_HANDLED;
}

static int __init hello_init(void)
{
	int errno, irq_line;
	if((errno = gpio_direction_input(GPIO)) !=0)
	{
		printk(KERN_INFO "Can't set GPIO direction, error %i\n", errno);
		gpio_free(GPIO);
		return -EINVAL;
	}
	irq_line = gpio_to_irq(GPIO);
	printk ("IRQ Line is %d \n",irq_line);

	errno = request_irq( irq_line, (irq_handler_t)irq_handler, NULL, "Interrupt123", NULL );
	irq_set_irq_type(irq_line, IRQ_TYPE_EDGE_RISING);
	if(errno<0)
	{
		printk(KERN_INFO "Problem requesting IRQ, error %i\n", errno);
	}
	printk(KERN_INFO "Module loaded successfully in irq line of %d\n", irq_line);
	return 0;
}

static void __exit hello_exit(void)
{
	int irq_line;
	printk ("Unloading my module.\n");
	irq_line = gpio_to_irq(GPIO);
	free_irq(irq_line,NULL);
	printk("Hello Example Exit\n");
	return;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("TheInventor");
MODULE_DESCRIPTION("Interrupt Module");
MODULE_LICENSE("GPL");
