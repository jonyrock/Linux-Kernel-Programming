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

void printAllGrouped()
{
    int i = 0;
    int tgids_count = 0;
    bool tgid_founded = false;
    struct task_struct* task;
    
    printk("\nAll tasks grouped:\n");
    for_each_process(task) {
        tgid_founded = false;
        for(i = 0; i < tgids_count; i++) {
            if(tgids_buffer[i] == task->tgid) {
                tgid_founded = true;
            }
        }
        if(tgid_founded) continue;
        tgids_buffer[tgids_count] = task->tgid;
        tgids_count++;
        if(MAX_ARRAY_LENGTH == tgids_count) break;
    }

    for(i = 0; i < tgids_count; i++) {
        printk("TGID: %d\n", tgids_buffer[i]);
        for_each_process(task) {
            if(task->tgid != tgids_buffer[i]) continue;
            printk("\t%d\n", task->pid);
        }
    }
}

static int __init start(void)
{
    printCurrent();
    printAll();
    printAllGrouped();
    return 0;
}

static void __exit stop(void)
{
    printk("\n    --end--    \n");
}
module_init(start);
module_exit(stop);

/******************************************************************************/