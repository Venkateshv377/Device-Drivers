#include<linux/module.h>
#include<linux/init.h>
#include <asm/io.h>

#define GPIO_NUMBER             48
#define GPIO_ADDR               0x4804C000

static int foo;
unsigned int gpio_logical_addr;

static ssize_t foo_show(struct kobject * kobj, struct kobj_attribute * attr, char * buf)
{
	return sprintf(buf, "%dn", foo);
}

static ssize_t foo_store(struct kobject * kobj, struct kobj_attribute * attr, const char * buf, size_t count)
{
	sscanf(buf, "%d", &foo);

	switch(*buf)
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
	
	gpio_logical_addr = ioremap(GPIO_ADDR, 0x400);  /* 0x4804C000 is the address of GPIO_48 pin */
	iowrite32(~(1 << 16), gpio_logical_addr + 0x134);

	printk(KERN_INFO "Module Initializing...\n");
	ex_kobj = kobject_create_and_add("gpio48", kernel_kobj);
	if(!ex_kobj)
		return ENOMEM;

	retval = sysfs_create_group(ex_kobj, &attr_group);
	if(retval)
		kobject_put(ex_kobj);
	return retval;
}

static void __exit ex_exit(void)
{
	iounmap(gpio_logical_addr);
	kobject_put(ex_kobj);
	printk(KERN_INFO "Module Uninitializing...\n");
}

module_init(ex_init);
module_exit(ex_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Venkatesh");
