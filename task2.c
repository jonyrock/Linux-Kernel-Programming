
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>

void printHW(void)
{

//    struct task_struct *task = $init_task;
    
//    list_for_each(list, current->children) {
//        task = list_entry(list, struct task_struct, sibling);
//        /* переменная task теперь указывает на один из процессов,
//        порожденных текущим процессом */
//    }


}

static int __init start(void)   // Точка входа в модуль
{
    printHW();                  // Вызов функции
    return 0;                   // в случае успешной загрузки возвращать нулевое значение
}

module_init(start);
//module_exit(stop);
MODULE_LICENSE("GPL");
