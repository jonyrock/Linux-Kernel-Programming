#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31

struct sock *nl_sk = NULL;
struct nlmsghdr *nlh;
int pid;
struct sk_buff *skb_out;
int res;

void send_message(char* msg, size_t msg_size, __u16 flags)
{
    skb_out = nlmsg_new(msg_size, 0);
    
    if(!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }
    
    nlh = nlmsg_put(skb_out, 0, 0, flags, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    strncpy(nlmsg_data(nlh), msg, msg_size);
    
    res = nlmsg_unicast(nl_sk, skb_out, pid);
    
    if(res < 0)
        printk(KERN_INFO "Error while sending bak to user\n");
}

void send_message_int(int t, __u16 flags)
{
    int* ptr;
    skb_out = nlmsg_new(sizeof(int), 0);
    //printk("%d\n", t);

    if(!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }
    
    nlh = nlmsg_put(skb_out, 0, 0, flags, sizeof(int), 0);
    NETLINK_CB(skb_out).dst_group = 0;
    ptr = (int*)nlmsg_data(nlh);
    *ptr = t;
    
    res = nlmsg_unicast(nl_sk, skb_out, pid);
    
    if(res < 0)
        printk(KERN_INFO "Error while sending bak to user\n");
}

void send_error(void)
{
    char* msg = "Error occured";
    send_message(msg, strlen(msg), NLMSG_ERROR);
}

void tgid_list(void)
{
    struct task_struct* task;
    for_each_process(task) {
        send_message_int((int)task->tgid, 0);
    }
}

void pids_by_tgid(int tgid)
{
    struct task_struct* task;
    for_each_process(task) {
        if((int)task->tgid == tgid)
            send_message_int((int)task->pid, 0);
    }
    
}

void comm_by_pid(int pid)
{
    struct task_struct* task;
    bool found = false;

    for_each_process(task) {
        if((int)task->pid == pid) {
            send_message(task->comm, strlen(task->comm), 0);
            found = true;
            break;
        }
    }
    if(found) return;
    send_error();
}

void recv_msg(struct sk_buff *skb)
{   
    int value;

    nlh=(struct nlmsghdr*)skb->data;
    pid = nlh->nlmsg_pid;    
    value = *(int*)nlmsg_data(nlh);
    printk("Get:%hd value:%d\n", nlh->nlmsg_flags, value);
    switch(nlh->nlmsg_flags) {
        case 1: 
            tgid_list();
            break;
        case 2:
            pids_by_tgid(value);
            break;
        case 3:
            comm_by_pid(value);
            break;
        default:
            send_error();
    }
    send_message(NULL, 0, NLMSG_DONE);
}


int __init start(void)
{
    printk("Service started\n");
    nl_sk=netlink_kernel_create(&init_net, NETLINK_USER, 0, recv_msg,
                                NULL, THIS_MODULE);
                                
    if(!nl_sk) {
        printk("Error creating socket.\n");
        return -10;
    }
    
    return 0;
}

static void __exit stop(void)
{
    if(nl_sk != 0) netlink_kernel_release(nl_sk);
}

module_init(start);
module_exit(stop);

MODULE_LICENSE("GPL");
