#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

// ================================================================================

#define NETLINK_TEST 17
#define VFW_GROUP 0
#define MSG_SIZE NLMSG_SPACE(1024)

static char buf[10240];

int main(int argc, char** argv)
{
    int nlsock;
    struct sockaddr_nl nla;
    
    nlsock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_TEST);
    
    if(nlsock < 0) {
        printf("Failed to create a Netlink socket: %s\n", strerror(errno));
        return 1;
    }
    
    nla.nl_family = AF_NETLINK;
    nla.nl_pad = 0;
    nla.nl_pid = 0;
    nla.nl_groups = 0;
    
    if(connect(nlsock, (const struct sockaddr*)&nla, sizeof(nla)) < 0) {
        printf("Failed to connect to the kernel rtnetlink: %s\n", strerror(errno));
        goto close_sock;
    }
    
    struct nlmsghdr* nlh = (struct nlmsghdr*)buf;
    nlh->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    nlh->nlmsg_type = RTM_GETLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = getpid();
    
    struct ifinfomsg* msg = (struct ifinfomsg*)NLMSG_DATA(nlh);
    
    msg->ifi_family = AF_UNSPEC;
    msg->ifi_index = 1;
    msg->ifi_change = 0xFFFFFFFF;
    
    if(send(nlsock, buf, nlh->nlmsg_len, 0) < 0) {
        printf("Failed to send a Netlink message: %s\n", strerror(errno));
        goto close_sock;
    }
    
    int sz = recv(nlsock, buf, sizeof(buf), 0);
    
    while(NLMSG_OK(nlh, sz) && nlh->nlmsg_type != NLMSG_DONE) {
        if(nlh->nlmsg_type & NLMSG_ERROR) {
            struct nlmsgerr* e = (struct nlmsgerr*)NLMSG_DATA(nlh);
            printf("Failed to get a link: %d (%s)\n", e->error, strerror(-e->error));
            goto close_sock;
        }
        
        if(nlh->nlmsg_type & RTM_NEWLINK) {
            struct rtattr* pattr;
            struct ifinfomsg* msg = (struct ifinfomsg*)NLMSG_DATA(nlh);
            
            pattr = (struct rtattr*)(msg + 1);
            int alen = (char*)(nlh + nlh->nlmsg_len) - (char*)pattr;
            
            while(RTA_OK(pattr, alen)) {
                if(pattr->rta_type == IFLA_IFNAME) {
                    printf("Interface %d, name %.*s\n", msg->ifi_index, RTA_PAYLOAD(pattr), RTA_DATA(pattr));
                }
                
                pattr = RTA_NEXT(pattr, alen);
            }
        }
        
        nlh = NLMSG_NEXT(nlh, sz);
    }
    
close_sock:
    close(nlsock);
    
    return 0;
}
