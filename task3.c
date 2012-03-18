/** Author Alexey Velikiy **/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("APTU.2012.PLK.503.AlexeyVelikiy.Task_3");

void printHW(void)
{
    
    printk("Helloo moto\n");
    
    struct task_struct *task;
    task = current;
    
    while(task->pid != 1){
        
        printk("%d\n", task->pid);
        task = task->parent;        
        
    }
    
    printk("%d\n", task->pid);
    
    
    
//    for(task = current; task != init_task; task = task->parent) {
//        
//        
//    
//    }
    
    
    
}

static int __init start(void)
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
