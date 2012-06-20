/******************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <net/sock.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 30

struct sock *nl_sk = NULL;

static void hello_nl_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    int pid;
    
    printk("Entering: %s\n", __FUNCTION__);
    
    nlh=(struct nlmsghdr*)skb->data;
    printk("Netlink received msg payload: %s\n",
           (char*)NLMSG_DATA(nlh));
    pid = nlh->nlmsg_pid; /*pid of sending process */
    NETLINK_CB(skb).dst_group = 0; /* not in mcast group */
    NETLINK_CB(skb).pid = 0;      /* from kernel */
    //NETLINK_CB(skb).groups = 0; /* not in mcast group */
    //NETLINK_CB(skb).dst_pid = pid;
    printk("About to send msg bak:\n");
    //netlink_unicast(nl_sk,skb,pid,MSG_DONTWAIT);
}



static int __init start(void)
{
    printk("Entering: %s\n",__FUNCTION__);
    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, 0,
                                hello_nl_recv_msg, NULL, THIS_MODULE);
                                
    if(!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }
    
    return 0;
}

static void __exit stop(void)
{
    printk("\n    --end--    \n");
    if(nl_sk != NULL)
        netlink_kernel_release(nl_sk);  
}
module_init(start);
module_exit(stop);

/******************************************************************************/
