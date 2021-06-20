#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/stop_machine.h>
#include <linux/delay.h>

#define DRIVER_NAME "stop-machine"

static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM = 1;
static unsigned int chardev_major;
static struct cdev chardev_cdev;
static struct class *chardev_class = NULL;

static unsigned long stop_time = 0;
static unsigned char buffer[256];

static int chardev_open(struct inode *inode, struct file *filp)
{
    printk("chardev_open\n");
    return 0;
}

static int chardev_release(struct inode *inode, struct file *filp)
{
    printk("chardev_release\n");
    return 0;
}

static int stop_func(void *arg)
{
    mdelay(stop_time);
    return 0;
}

static ssize_t chardev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    printk("stop machine\n");  

    if (copy_from_user(buffer, buf, count) != 0) {
        return -EFAULT;
    }

    if(kstrtol(buffer, 0, &stop_time) != 0) {
        printk(KERN_ERR "Failed to kstrtol\n");
        return -1;
    }

    if (stop_machine(stop_func, NULL, NULL) != 0) {
        printk(KERN_ERR "Failed to stop_machine\n");
        return -1;
    }
    
    return count;
}

struct file_operations char_device_fops = {
    .owner = THIS_MODULE,
    .open = chardev_open,
    .release = chardev_release,
    .write = chardev_write,
};

static __init int chardev_init(void)
{   
    int is_alloc_region = 0, cdev_add_err = 0;
    dev_t dev;

    printk("chardev_init\n");

    is_alloc_region = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DRIVER_NAME);
    if (is_alloc_region != 0) {
        printk(KERN_ERR "Failed to alloc_chrdev_region\n");
        return -1;
    }

    chardev_major = MAJOR(dev);

    cdev_init(&chardev_cdev, &char_device_fops);
    chardev_cdev.owner = THIS_MODULE;

    cdev_add_err = cdev_add(&chardev_cdev, dev, MINOR_NUM);
    if (cdev_add_err != 0) {
        printk(KERN_ERR "Failed to cdev_add\n");
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    chardev_class = class_create(THIS_MODULE, DRIVER_NAME);
    if (IS_ERR(chardev_class)) {
        printk(KERN_ERR "Failed to class_create\n");
        cdev_del(&chardev_cdev);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }
	
    device_create(chardev_class, NULL, MKDEV(chardev_major, MINOR_BASE), NULL, DRIVER_NAME);

	return 0;
}

static __exit void chardev_exit(void)
{
	dev_t dev = MKDEV(chardev_major, MINOR_BASE);
	
	printk("chardev_exit\n");
	
	device_destroy(chardev_class, MKDEV(chardev_major, MINOR_BASE));

	class_destroy(chardev_class);
	cdev_del(&chardev_cdev);
	unregister_chrdev_region(dev, MINOR_NUM);
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_AUTHOR("fumiya");
MODULE_LICENSE("Dual BSD/GPL");