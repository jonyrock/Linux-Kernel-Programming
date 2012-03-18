/** Author Alexey Velikiy **/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("APTU.2012.PLK.503.AlexeyVelikiy.Task_3");

// find init process
struct task_struct* findInitTask(void);

void printProcessSubtree(struct task_struct* task, size_t padding)
{

    struct task_struct* taskInner;
    struct list_head *list;
    size_t i;
    
    if(task == NULL) {
        printk("Null pointer");
        return;
    }
    
    
    for(i = 0; i < padding; i++) {
        printk("    ");
    }
    
    printk("%s[%d]\n", task->comm, task->pid);
    
    
    list_for_each(list, *task->children) {
//
//      task1 = list_entry(list, struct task_struct, sibling);
//      printProcessSubtree(task1, padding + 1);
//
    }
    
}


static int __init start(void)
{
    struct task_struct* initTask = findInitTask();
    printProcessSubtree(initTask, 0);
    return 0;
}

static void __exit stop(void) {}

module_init(start);
module_exit(stop);



/**************************/
/*** addition functions ***/
/**************************/

struct task_struct* findInitTask(void) {

    struct task_struct *task;
    task = current;
    
    while(task->pid != 1) {
        task = task->parent;
    }
    
    return task;
}
