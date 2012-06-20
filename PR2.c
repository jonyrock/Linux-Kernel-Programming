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

void sendMessage(char* msg, size_t msg_size, __u16 flags)
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

void recv_msg(struct sk_buff *skb)
{
    
    nlh=(struct nlmsghdr*)skb->data;
    pid = nlh->nlmsg_pid;    
    char* value = (char*)nlmsg_data(nlh);
    if(*value == 1)
        printk("Hello one!");
    if(*value == 2)
        printk("Hello two!");

    char* msg = "Oh it's cool";
    sendMessage(msg, strlen(msg), NLMSG_DONE);
    
}


int __init start(void)
{

    printk("Service started\n");
    nl_sk=netlink_kernel_create(&init_net, NETLINK_USER, 0, recv_msg,
                                NULL, THIS_MODULE);
                                
    if(!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
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
