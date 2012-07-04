#include <linux/sysfs_pslist.h>
#include <asm/page.h>

#include <linux/sched.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/printk.h>

// --ATTRS-------------------------------------------------------------------------
static ssize_t name_show(struct kobject *kobj, 
                                struct kobj_attribute *attr, 
                                char *buf) {
    strncpy(buf,
        container_of(kobj, struct task_struct, pslist_link)->comm, 
        TASK_COMM_LEN);

    return 0;
}

static struct kobj_attribute process_name_attr = __ATTR_RO(name);

static struct attribute* process_attrs[] = { &process_name_attr.attr, NULL };

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


// ********************************************************************************
struct task_struct* task_init_log[MAX_LOG_VALUE];
int task_init_log_count = 0;
struct task_struct* task_link_log[MAX_LOG_VALUE * 2];
int task_link_log_count = 0;
struct task_struct* task_release_log[MAX_LOG_VALUE];
int task_release_log_count = 0;
bool sysfs_exist = false;
static struct kobject kobj_root;

// --HANDLERS----------------------------------------------------------------------

void pslist_task_init(struct task_struct *child)
{   
    int res = 0;
    if(!sysfs_exist) {
        task_init_log[task_init_log_count++] = child;
        if(task_init_log_count >= MAX_LOG_VALUE) task_init_log_count = 0;
    } else {
        memset(&child->pslist_link, 0, sizeof(struct kobject));
        kobject_init(&child->pslist_link, &process_kobj_type);
        res = kobject_add(&child->pslist_link, &kobj_root, "%d", child->pid);
    }
} 

void pslist_task_link(struct task_struct *parent, struct task_struct *child) 
{
    int res = 0;
    if(!sysfs_exist) {
        task_link_log[task_link_log_count] = child;
        task_link_log[task_link_log_count + 1] = child;
        task_link_log_count += 2;
        if(task_link_log_count >= MAX_LOG_VALUE * 2) task_link_log_count = 0;
    } else {
       res = sysfs_create_link(&parent->pslist_link, 
                         &child->pslist_link, 
                         kobject_name(&child->pslist_link));
    }
}

void pslist_task_release(struct task_struct *child) 
{
    struct task_struct* taskp;
    if(!sysfs_exist) {
        task_release_log[task_release_log_count++] = child;
        if(task_release_log_count >= MAX_LOG_VALUE) task_release_log_count = 0;
    } else {
        kobject_del(&child->pslist_link);
        kobject_put(&child->pslist_link);
        for_each_process(taskp) {
            sysfs_remove_link(&taskp->pslist_link, 
                kobject_name(&child->pslist_link));
        }
    }
}

static void pslist_init_logs(void) 
{ 
    int rel_i = 0;
    int j = 0;
    for(rel_i = 0; rel_i < task_release_log_count; rel_i++) {
        for(j = 0; j < task_init_log_count; j++) {
            if(task_init_log[j] == task_release_log[rel_i])
                task_init_log[j] = NULL;
        }
        for(j = 0; j < task_link_log_count; j+= 2) {
            if( task_link_log[j]     == task_release_log[rel_i] || 
                task_link_log[j + 1] == task_release_log[rel_i] ) {
                    task_link_log[j] = task_link_log[j + 1] = NULL;
            }
        }
    }
    
    for(j = 0; j < task_init_log_count; j++) {
        if(task_init_log[j] == NULL) continue;
        pslist_task_init(task_init_log[j]);
    }
    
    for(j = 0; j < task_link_log_count; j+= 2) {
        if(task_link_log[j] == NULL) continue;
        pslist_task_link(task_link_log[j], task_link_log[j + 1]);
    }


}

int pslist_init(void) 
{
    int res = 0;
    kobject_init(&kobj_root, &root_type);
    res = kobject_add(&kobj_root, kernel_kobj, "pslist");
    if(res < 0) {
        printk("Init pslist failure!\n");
        return -10;
    }

    sysfs_exist = true;
    pslist_init_logs();
    return 0;
}

