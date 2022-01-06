// https://tldp.org/HOWTO/Multicast-HOWTO-6.html
//
// Example:
// ./network_multicast_listener 239.255.255.251 27335 192.168.88.50
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc < 3) {
       printf("Command line args should be multicast group and port and [interface] optional\n");
       printf("(e.g. for SSDP, `listener 239.255.255.250 1900 [192.168.1.1]`)\n");
       return 1;
    }

    const char* group = argv[1]; // e.g. 239.255.255.250 for SSDP
    const int port = atoi(argv[2]); // 0 if error, which is an invalid port
    const char* source_iface = (argc == 4) ? argv[3] : NULL;

    //
    // create what looks like an ordinary UDP socket
    //
    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    //
    // allow multiple sockets to use the same PORT number
    //
    const u_int yes = 1;
    if (
        setsockopt(
            fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)
        ) < 0
    ) {
       perror("Reusing ADDR failed");
       return 1;
    }

    //
    // set up destination address
    //
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(group);
    addr.sin_port = htons((unsigned short)port);

    //
    // bind to receive address
    //
    if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    //
    // use setsockopt() to request that the kernel join a multicast group
    //
    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = source_iface ? inet_addr(source_iface) : htonl(INADDR_ANY);
    if (
        setsockopt(
            fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)
        ) < 0
    ) {
        perror("setsockopt");
        return 1;
    }

    //
    // now just enter a read-print loop
    //
    char msgbuf[4096];
    while (1) {
        unsigned addrlen = sizeof(addr);
        ssize_t const nbytes = recvfrom(
            fd,
            msgbuf,
            sizeof(msgbuf),
            0,
            (struct sockaddr*) &addr,
            &addrlen
        );
        if (nbytes < 0) {
            perror("recvfrom");
            return 1;
        }
        msgbuf[nbytes] = '\0';
        printf("from: %s message: %s\n", inet_ntoa(addr.sin_addr), msgbuf);
     }

    return 0;
}
