
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/list.h>

MODULE_AUTHOR("Alexey Velikiy");
MODULE_DESCRIPTION("APTU.2012.PLK.503.AlexeyVelikiy.Task3");
MODULE_LICENSE("GPL");

/**
 * Finding init proccess with pid = 1
 * from current by parents.
 */
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
    
        
    printk("%s[%d:%d]\n", task->comm, task->pid, task->tgid);
    
    
    list_for_each(list, &task->children) {
        taskInner = list_entry(list, struct task_struct, sibling);
        printProcessSubtree(taskInner, padding + 1);
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


/***************************************************************/
/****************** function implementations *******************/
/***************************************************************/


struct task_struct* findInitTask(void) {

    struct task_struct *task;
    task = current;
    
    while(task->pid != 1) {
        task = task->parent;
    }
    
    return task;
}
