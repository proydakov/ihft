// https://tldp.org/HOWTO/Multicast-HOWTO-6.html
//
// Example:
// ./network_multicast_listener 239.255.255.251 27335 192.168.88.50
//

#include "multicast_helper.h"

#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc < 3) {
       printf("Command line args should be multicast group and port and [interface] optional\n");
       printf("(e.g. for SSDP, `listener 239.255.255.250 1900 [192.168.1.1]`)\n");
       return 1;
    }

    const char* const group = argv[1]; // e.g. 239.255.255.250 for SSDP
    const unsigned short port = (unsigned short) atoi(argv[2]); // 0 if error, which is an invalid port
    const char* const source_iface = (argc == 4) ? argv[3] : NULL;
    
    auto const opt = create_multicast_listener(group, port, source_iface);
    if (!opt)
    {   
        return 1;
    }
    
    const int fd = opt.value();

    //
    // now just enter a read-print loop
    //
    struct sockaddr_in addr{};
    char msgbuf[4096];
    while (true) {
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
