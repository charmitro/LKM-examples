/* LKM Kthread Example.
 * To run:
 *   make
 *   sudo insmod kthread_example.ko
 *   sudo dmesg -w
 *   sudo rmmod kthread_example
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/err.h>

dev_t dev = 0;
static struct class *dev_class;
static struct cdev kthread_example_cdev;

static int __init kthread_example_driver_init(void);
static void __exit kthread_example_driver_exit(void);

static struct task_struct *kthread_example_thread;

/* Function prototypes */
static int kthread_example_open(struct inode *inode, struct file *file);
static int kthread_example_release(struct inode *inode, struct file *file);
static ssize_t kthread_example_read(struct file *filp, char __user *buf,
				    size_t len, loff_t *off);
static ssize_t kthread_example_write(struct file *filp, const char *buf,
				     size_t len, loff_t *off);
int thread_function(void *pv);

/********/
int thread_function(void *pv)
{
	int i = 0;
	while (!kthread_should_stop()) {
		pr_info("In kthread_example thread function %d\n", i++);
		msleep(1000);
	}

	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = kthread_example_read,
	.write = kthread_example_write,
	.open = kthread_example_open,
	.release = kthread_example_release,
};

static int kthread_example_open(struct inode *inode, struct file *file)
{
	pr_info("Deivce file opened.\n");
	return 0;
}

static int kthread_example_release(struct inode *inode, struct file *file)
{
	pr_info("Device file closed.\n");
	return 0;
}
static ssize_t kthread_example_read(struct file *filp, char __user *buf,
				    size_t len, loff_t *off)
{
	pr_info("Read function\n");
	return 0;
}

static ssize_t kthread_example_write(struct file *filp, const char *buf,
				     size_t len, loff_t *off)
{
	pr_info("Write function\n");
	return len;
}

static int __init kthread_example_driver_init(void)
{
	if ((alloc_chrdev_region(&dev, 0, 1, "kthread_example_Dev")) < 0) {
		pr_err("Cannot allocate major number\n");
		return -1;
	}
	pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

	cdev_init(&kthread_example_cdev, &fops);

	if ((cdev_add(&kthread_example_cdev, dev, 1)) < 0) {
		pr_err("Cannot add the device to the system\n");
		goto r_class;
	}

	if (IS_ERR(dev_class = class_create(THIS_MODULE,
					    "kthread_example_class"))) {
		pr_err("Cannot create struct class\n");
		goto r_class;
	}

	if (IS_ERR(device_create(dev_class, NULL, dev, NULL,
				 "kthread_example_device"))) {
		pr_err("Cannot create the device\n");
		goto r_device;
	}

	kthread_example_thread =
		kthread_create(thread_function, NULL, "kthread Thread");
	if (kthread_example_thread) {
		wake_up_process(kthread_example_thread);
	} else {
		pr_err("Cannot create kthread\n");
		goto r_device;
	}

#if 0
	kthread_example_thread = kthread_run(thread_function, NULL, "kthread Thread");
	if (kthread_example_thread) {
		pr_info("kthread created.\n");
	} else {
		pr_err("Cannot create kthread\n");
		goto r_device;
	}
#endif

	pr_info("Device driver insert...done.\n");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev, 1);
	cdev_del(&kthread_example_cdev);
	return -1;
}

static void __exit kthread_example_driver_exit(void)
{
	kthread_stop(kthread_example_thread);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&kthread_example_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("Device sdriver remote...done.\n");
}

module_init(kthread_example_driver_init);
module_exit(kthread_example_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charalampos Mitrodimas <charmitro@posteo.net>");
MODULE_DESCRIPTION("Kthread LKM example");
