/* LKM Parameters example.
 * To run:
 *   make
 *   sudo insmod params.ko short_param=3 int_param=1 array_param=13,13,13
 *   sudo dmesg -w
 *   sudo rmmod params
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/stat.h>

#define MODULE_NAME "params"

static short int short_param = 1;
static int int_param = 128;
static char *str_param = "string";

/**
 * module_param - typesafe helper for a module/cmdline parameter
 * @name: the variable to alter, and exposed parameter name.
 * @type: the type of the parameter
 * @perm: visibility in sysfs.
 */
module_param(short_param, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(short_param, "A short integer");
module_param(int_param, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(int_param, "An integer");
module_param(str_param, charp, 0000);
MODULE_PARM_DESC(str_param, "A character string");

static int array_param[2] = { 128, 128 };
static int array_argc = 0;

/**
 * module_param_array - a parameter which is an array of some type
 * @name: the name of the array variable
 * @type: the type, as per module_param()
 * @nump: optional pointer filled in with the number written
 * @perm: visibility in sysfs
 */
module_param_array(array_param, int, &array_argc, 0000);
MODULE_PARM_DESC(array_param, "An array of integers");

static int __init params_init(void);
static void __exit params_exit(void);

static int __init params_init(void)
{
	int i = 0;

	pr_info("Hello World! Sent from '%s' module\n", MODULE_NAME);
	pr_info("short_param is a short integer: %hd\n", short_param);
	pr_info("short_param is an integer: %d\n", int_param);
	pr_info("short_param is a string: %s\n", str_param);

	for (i = 0; i < ARRAY_SIZE(array_param); i++)
		pr_info("array_param[%d] = %d\n", i, array_param[i]);

	pr_info("Got %d arguments for array_param.\n", array_argc);
	return 0;
}

static void __exit params_exit(void)
{
	pr_info("Goodbye World! Sent from '%s' module\n", MODULE_NAME);
}

module_init(params_init);
module_exit(params_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charalampos Mitrodimas <charmitro@posteo.net>");
MODULE_DESCRIPTION("LKM Example Module Parameters");
