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

#define NETLINK_TEST 17
#define VFW_GROUP 0
#define MSG_SIZE NLMSG_SPACE(1024)

static struct sock *nl_sk = NULL;

static void nltest_rcv(struct sock *sk, int len)
{
    struct sk_buff *nl_skb;
    struct nlmsghdr *nl_hdr;
    int pid;
    
    while((nl_skb = skb_dequeue(&sk->sk_receive_queue)) != NULL) {
        nl_hdr = (struct nlmsghdr *)nl_skb->data;
        pid = nl_hdr->nlmsg_pid;
        printk(KERN_ALERT "*** Message from user with PID: (pid = %d) is %s\n",
               pid, (char*)NLMSG_DATA(nl_hdr));
        nl_skb = alloc_skb(MSG_SIZE, in_interrupt() ?
                           GFP_ATOMIC : GFP_KERNEL);
        skb_put(nl_skb, MSG_SIZE);
        nl_hdr = (struct nlmsghdr *)nl_skb->data;
        nl_hdr->nlmsg_len = MSG_SIZE;
        nl_hdr->nlmsg_pid = pid;
        nl_hdr->nlmsg_flags = 0;
        strcpy(NLMSG_DATA(nl_hdr), "HELLO HELLO HELLO");
        NETLINK_CB(nl_skb).pid = 0;
        NETLINK_CB(nl_skb).dst_group = VFW_GROUP;
        netlink_unicast(nl_sk, nl_skb, pid, 0);
        kfree_skb(nl_skb);
    }
}

static int __init start(void)
{
    struct net *net;
    printk(KERN_ALERT "INIT/START: nltest\n");
    nl_sk = netlink_kernel_create(net, NETLINK_TEST, VFW_GROUP, nltest_rcv, 0, THIS_MODULE);
    
    if(!nl_sk) {
        printk(KERN_ALERT "ERROR: nltest - netlink_kernel_create() failed\n");
        return -1;
    }
    
    return 0;
}

static void __exit stop(void)
{       
    printk(KERN_ALERT "EXIT: nltest\n");
    sock_release(nl_sk->sk_socket);
    //printk("\n    --end--    \n");
}
module_init(start);
module_exit(stop);

/******************************************************************************/
