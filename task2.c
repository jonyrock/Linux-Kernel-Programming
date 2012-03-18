
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>

void printHW(void)
{

    struct task_struct *task;
    for_each_process(task) {
        printk("%s [%d]\n", task->comm, task->pid);
    }
    
}

static int __init start(void)   // Точка входа в модуль
{
    printHW();                  // Вызов функции
    return 0;                   // в случае успешной загрузки возвращать нулевое значение
}

static int __exit stop(void){}

module_init(start);
module_exit(stop);
MODULE_LICENSE("GPL");
