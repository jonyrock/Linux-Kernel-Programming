/******************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#define MAX_ARRAY_LENGTH 10000

int tgids_buffer[MAX_ARRAY_LENGTH];

void printCurrent(void)
{
    printk("Current task info:\n");
    printk("Name: %s\nPID: %d\nTGID: %d\n",
           current->comm, current->pid, current->tgid);
}

void printAll(void)
{
    struct task_struct* task;
    printk("\nAll tasks info:\n");
    printk("PID\tTGID\tCOMM\n");
    for_each_process(task) {
        printk("%d\t%d\t%s\n", task->pid, task->tgid, task->comm);
    }
}

void printAllGrouped(){
    
}

static int __init start(void)
{
    printCurrent();
    printAll();
    return 0;
}

static void __exit stop(void)
{
    printk("\n    --end--    \n");
}
module_init(start);
module_exit(stop);

/******************************************************************************/
