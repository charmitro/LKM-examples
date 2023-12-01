/* LKM Hello World Example.
 * To run:
 *   make
 *   sudo insmod hello_world.ko
 *   sudo dmesg -w
 *   sudo rmmod hello_world
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

#define MODULE_NAME "hello_world"

// The `__init` macro will make this function be discarded and its memory
// freed once it finishes. This only works for build-in drivers, not
// loadable modules.
static int __init hello_world_init(void);
// The `__exit` macro causes the omission of this function. Again, this has
// no effect for loadable modules.
static void __exit hello_world_exit(void);

static int __init hello_world_init(void)
{
	pr_info("Hello World! Sent from '%s' module\n", MODULE_NAME);

	return 0;
}

static void __exit hello_world_exit(void)
{
	pr_info("Goodbye World! Sent from '%s' module\n", MODULE_NAME);
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charalampos Mitrodimas <charmitro@posteo.net>");
MODULE_DESCRIPTION("Hello world Example of a Linux Kernel Module");
