/******************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>

static int __init start(void)
{
    return 0;
}

static void __exit stop(void)
{
    printk("\n    --end--    \n");
}

module_init(start);
module_exit(stop);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Velikiy <gmpota@gmail.com>");




/******************************************************************************/

