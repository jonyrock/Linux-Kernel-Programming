/** Author Alexey Velikiy **/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/list.h>

void printHW(void)
{
    printk(KERN_CRIT "Hello,Debugg world 333 %s, pid %d \n", 
        current->comm, 
        current->pid);        
            
}

static int start(void)
{
    printHW();
    return 0;
}

static void __exit stop(void)
{
    printk("Module unloaded\n");
}
module_init(start);
module_exit(stop);
MODULE_LICENSE("GPL");
