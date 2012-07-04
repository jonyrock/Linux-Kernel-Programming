/******************************************************************************/
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

char name_buffer[40];
static struct kobject *root_kobj;

static ssize_t name_show(struct kobject *kobj, 
                                struct kobj_attribute *attr, 
                                char *buf) {
    strncpy(buf,
        container_of(kobj, struct task_struct, pslist_link)->comm, 
        TASK_COMM_LEN);

    return 0;
}

static struct kobj_attribute process_name_attr  = __ATTR_RO(name);

static struct attribute* process_attrs[] = {
       &process_name_attr.attr,
       NULL
};

static struct kobj_type process_kobj_type = {
       .release = NULL,
       .sysfs_ops = &kobj_sysfs_ops,
       .default_attrs = process_attrs,
       .child_ns_type = NULL,
       .namespace = NULL
};

static struct kobj_type root_type = {
       .release = NULL,
       .sysfs_ops = &kobj_sysfs_ops,
       .default_attrs = NULL,
       .child_ns_type = NULL,
       .namespace = NULL
};

static struct kobject kobj_root;

void pslist_task_init(struct task_struct *child) {

       memset(&child->pslist_link, 0, sizeof(struct kobject));

       kobject_init(&child->pslist_link, &process_kobj_type);
       kobject_add(&child->pslist_link, &kobj_root, "%d", child->pid);
} 

void pslist_task_link(struct task_struct *parent, struct task_struct *child) {
       sysfs_create_link(&parent->pslist_link, 
                         &child->pslist_link, 
                         kobject_name(&child->pslist_link));
}

void pslist_task_release(struct task_struct *tsk) {
       kobject_del(&tsk->pslist_link);
       kobject_put(&tsk->pslist_link);
}

static int __init pslist_init(void)
{
      kobject_init(&kobj_root, &root_type);
      kobject_add(&kobj_root, kernel_kobj, "pslist");

      //initialized = 1;

}

static void __exit stop(void)
{
    printk("Exit\n");
    kobject_put(root_kobj);
}

module_init(pslist_init);
module_exit(stop);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Velikiy <gmpota@gmail.com>");
















/******************************************************************************/