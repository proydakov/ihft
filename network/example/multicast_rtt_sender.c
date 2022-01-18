// https://tldp.org/HOWTO/Multicast-HOWTO-6.html
//
// Example:
// ./network_multicast_sender 239.255.255.251 27335 192.168.88.50
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
    if (argc < 3) {
       printf("Command line args should be multicast group and port\n");
       printf("(e.g. for SSDP, `sender 239.255.255.250 1900 [interface_ip]`)\n");
       return 1;
    }

    const char* group = argv[1]; // e.g. 239.255.255.250 for SSDP
    const unsigned short port = (unsigned short) atoi(argv[2]); // 0 if error, which is an invalid port
    const char* source_iface = (argc == 4 ? argv[3] : NULL);

    //
    // create what looks like an ordinary UDP socket
    //
    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_interface.s_addr = source_iface ? inet_addr(source_iface) : htonl(INADDR_ANY);

    if (
        setsockopt(
            fd, IPPROTO_IP, IP_MULTICAST_IF, (char*) &mreq, sizeof(mreq)
        ) < 0
    ) {
        perror("setsockopt");
        return 1;
    }

    //
    // set up destination address
    //
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(group);
    addr.sin_port = htons(port);

    //
    // now just sendto() our destination!
    //
    for (unsigned i = 0; ; i++) {

        struct timespec tp;
        clock_gettime(CLOCK_REALTIME, &tp);
        long const start_micro = tp.tv_sec * 1000 * 1000 * 1000 + tp.tv_nsec;

        ssize_t const nbytes = sendto(
            fd,
            &start_micro,
            sizeof(start_micro),
            0,
            (struct sockaddr*) &addr,
            sizeof(addr)
        );
        if (nbytes < 0) {
            perror("sendto");
            return 1;
        }

        //
        // !!! If test requires, make these configurable via args
        //
        const int delay_secs = 1;

        sleep(delay_secs); // Unix sleep is seconds
    }

    return 0;
}
