#include <sys/socket.h>
#include <linux/netlink.h>

#define NETLINK_USER 31

#define MAX_PAYLOAD 32 /* maximum payload size*/
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;

struct msghdr msg;

void init()
{
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */
    
    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
    
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    
}

void send_message(short int flags, int msg_value)
{
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 1;
    
    int* dataPtr = (int*)NLMSG_DATA(nlh);
    *dataPtr = msg_value;
    
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = flags;
    sendmsg(sock_fd, &msg, 0);
}

void list_response()
{
    while(nlh->nlmsg_type != NLMSG_DONE){
        recvmsg(sock_fd, &msg, 0);
        int i = *(int*)NLMSG_DATA(nlh);
        printf("%d\n", i);
    }
}

void tgid_list()
{
    send_message(1, 0);
    list_response();
}

void pids_by_tgid(int tgid)
{
    send_message(2, tgid);
    list_response();
}

void main(char* args[])
{
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if(sock_fd < 0) {
        printf("Can't open socket!\n");
        return -1;
    }
    init();

    tgid_list();
    //pids_by_tgid(1);
    
    close(sock_fd);
    free(nlh);
}












