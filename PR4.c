#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

static int foo;
char name_buffer[40];

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
    return sprintf(buf, "%d\n", foo);
}

// Really do nothing. User can't update process name 
static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count){ return 0; }

static struct kobj_attribute foo_attribute = __ATTR(foo, 0666, foo_show, foo_store);

static struct attribute *attrs[] = {
    &foo_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *root_kobj;


void pslist_task_init(struct task_struct *task) {
    sprintf(name_buffer, "%d", task->pid);
    kobject_create_and_add(name_buffer, root_kobj);
} 

static int __init start(void)
{
    int retval;
    struct task_struct* task;
    
    root_kobj = kobject_create_and_add("pslist", kernel_kobj);
    if (!root_kobj)
            return -ENOMEM;
    
//    retval = sysfs_create_group(root_kobj, &attr_group);
//    if (retval)
//            kobject_put(root_kobj);
    
    for_each_process(task) {
        //printk(task-)
        pslist_task_init(task);
    }

    return retval;
}

static void __exit stop(void)
{
    kobject_put(root_kobj);
}

module_init(start);
module_exit(stop);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Velikiy <gmpota@gmail.com>");








