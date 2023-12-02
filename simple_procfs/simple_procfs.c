/* LKM procfs Example
 * To run:
 *   make
 *   sudo su
 *   insmod simple_procfs.ko
 *   echo "a string here" > /proc/simple_procfs
 *   cat /proc/simple_procfs
 *   rmmod simple_procfs
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/proc_fs.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROCFS_MAX_SIZE 1024
#define procfs_name "simple_procfs"

static struct proc_dir_entry *simple_proc_file;

static char procfs_buffer[PROCFS_MAX_SIZE];
static unsigned long procfs_buffer_size = 0;

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer,
			     size_t buffer_length, loff_t *offset)
{
	if (*offset || procfs_buffer_size == 0) {
		pr_debug("procfs_read: END\n");
		*offset = 0;
		return 0;
	}

	procfs_buffer_size = min(procfs_buffer_size, buffer_length);
	if (copy_to_user(buffer, procfs_buffer, procfs_buffer_size))
		return -EFAULT;
	*offset += procfs_buffer_size;

	pr_info("procfile read %s and %lu bytes\n",
		file_pointer->f_path.dentry->d_name.name, procfs_buffer_size);

	return procfs_buffer_size;
}

static ssize_t procfile_write(struct file *file, const char __user *buffer,
			      size_t len, loff_t *off)
{
	procfs_buffer_size = min((unsigned long)PROCFS_MAX_SIZE, len);

	if (copy_from_user(procfs_buffer, buffer, procfs_buffer_size))
		return -EFAULT;
	*off += procfs_buffer_size;

	pr_info("procfile write %s & %lu\n", procfs_buffer, procfs_buffer_size);
	return procfs_buffer_size;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
	.proc_read = procfile_read,
	.proc_write = procfile_write,
};
#else
static const struct file_operations proc_file_fops = {
	.read = procfile_read,
	.write = procfile_write,
};
#endif

static int __init simple_procfs_init(void)
{
	simple_proc_file =
		proc_create(procfs_name, 0644, NULL, &proc_file_fops);
	if (NULL == simple_proc_file) {
		proc_remove(simple_proc_file);
		pr_alert("Error: Could not initialise /proc/%s\n", procfs_name);
		return -ENOMEM;
	}

	pr_info("/proc/%s created\n", procfs_name);

	return 0;
}

static void __exit simple_procfs_exit(void)
{
	proc_remove(simple_proc_file);
	pr_info("/proc/%s removed\n", procfs_name);
}

module_init(simple_procfs_init);
module_exit(simple_procfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charalampos Mitrodimas <charmitro@posteo.net>");
MODULE_DESCRIPTION("LKM Character Device Example");
