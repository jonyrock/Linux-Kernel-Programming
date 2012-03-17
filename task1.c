
#include <linux/module.h>       // Этот файл подключается в любом модуле по соглашению
#include <linux/kernel.h>       // Содержит макросы для функции printk()
#include <linux/init.h>         // Содержит определения макросов __init и __exit
#include <linux/sched.h>

void printHW(void)              // Функция для вывода приветствия
{

    printk("Hello, world %s, pid %d \n", current->comm, current->pid);

}

EXPORT_SYMBOL(printHW);         // Экспорт функций ядра - предоставляет доступ к функции 
                                // другим модулям ядра

static int __init start(void)   // Точка входа в модуль
{
    printHW();                  // Вызов функции
    return 0;                   // в случае успешной загрузки возвращать нулевое значение
}

static void __exit stop(void)   // Точка выхода
{
    printk("Module unloaded\n");
}
module_init(start);             // When module loaded
module_exit(stop);              // When rmmod 
MODULE_LICENSE("GPL");          // Указывает на лицензию, под которой распространяется данный модуль
