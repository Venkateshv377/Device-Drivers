#include<linux/module.h>
#include<linux/init.h>
#include <asm/io.h>

#define GPIO_NUMBER             48
#define GPIO_ADDR               0x4804C000

static int foo;

static ssize_t foo_show(struct kobject * kobj, struct kobj_attribute * attr, char * buf)
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

	return sprintf(buf, "%dn", foo);
}

static ssize_t foo_store(struct kobject * kobj, struct kobj_attribute * attr, const char * buf, size_t count)
{
	gpio_logical_addr = ioremap(GPIO_ADDR, 0x400);  /* 0x4804C000 is the address of GPIO_48 pin */
	iowrite32(~(1 << 16), gpio_logical_addr + 0x134);

	sscanf(buf, "%du", &foo);
	return count;
}

static struct kobj_attribute foo_attribute = __ATTR(foo, 0660, foo_show, foo_store);

static struct attribute * attrs [] =
{
	&foo_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *ex_kobj;
static int __init ex_init(void)
{
	int retval;
	printk(KERN_INFO "i am loading...n");
	ex_kobj = kobject_create_and_add("ex", kernel_kobj);
	if(!ex_kobj)
		return ENOMEM;

	retval = sysfs_create_group(ex_kobj, &attr_group);
	if(retval)
		kobject_put(ex_kobj);
	return retval;
}

static void __exit ex_exit(void)
{
	kobject_put(ex_kobj);
	printk(KERN_INFO "i am unloading...n");
}

module_init(ex_init);
module_exit(ex_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ravi teja");
