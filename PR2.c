/******************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>

void printCurrent(void)
{
    printk("Current task info:\n");
    printk("Hello, world %s, pid %d, tgid \n",
           current->comm, current->pid, current->tgid);
}

void printAll(void)
{
    struct task_struct* task;
    printk("All tasks info:\n");
    printk("PID\tTGID\tCOMM\n");
    for_each_process(task) {
        printk("%d\t%d\t%s\n", task->pid, task->tgid, task->comm);
    }
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
