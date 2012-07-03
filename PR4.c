/******************************************************************************/
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

char name_buffer[40];
static struct kobject *root_kobj;

static ssize_t process_name_show(struct kobject *kobj, 
                                struct kobj_attribute *attr, 
                                char *buf) {
    //sprintf(buf, "Hello name");
//       strncpy(buf,
//               container_of(kobj, struct task_struct, comm), 
//               TASK_COMM_LEN);
    return 0;
}

static struct kobj_attribute process_name_attr  = __ATTR_RO(process_name);

void pslist_task_init(struct task_struct* task) {
    struct kobject * new_task_ptr;
    int res = 0;
    sprintf(name_buffer, "%d", task->pid);
    
    new_task_ptr = kobject_create_and_add(name_buffer, root_kobj);
    if(new_task_ptr == NULL) return;
    res = sysfs_create_file(new_task_ptr, &process_name_attr);
} 

static int __init start(void)
{
    int retval;
    struct task_struct* task;
    int q = 0;
    
    root_kobj = kobject_create_and_add("pslist5", kernel_kobj);
    if (!root_kobj) return -ENOMEM;
    
//    retval = sysfs_create_group(root_kobj, &attr_group);
//    if (retval)
//            kobject_put(root_kobj);
    
    
//    for_each_process(task) {
//        pslist_task_init(task);
//        if(q++ > 10) break;
//    }

    return retval;
}

static void __exit stop(void)
{
    printk("Exit\n");
    kobject_put(root_kobj);
}

module_init(start);
module_exit(stop);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Velikiy <gmpota@gmail.com>");
















/******************************************************************************/