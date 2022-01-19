// https://tldp.org/HOWTO/Multicast-HOWTO-6.html
//
// Example:
// ./network_multicast_sender 239.255.255.251 27335 192.168.88.50
//

#include "multicast_helper.h"

#include <time.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc < 3) {
       printf("Command line args should be multicast group and port\n");
       printf("(e.g. for SSDP, `sender 239.255.255.250 1900 [interface_ip]`)\n");
       return 1;
    }

    const char* const group = argv[1]; // e.g. 239.255.255.250 for SSDP
    const unsigned short port = (unsigned short) atoi(argv[2]); // 0 if error, which is an invalid port
    const char* const source_iface = (argc == 4 ? argv[3] : NULL);

    auto const opt = create_multicast_sender(source_iface);
    if (!opt)
    {
        return 1;
    }

    const int fd = opt.value();

    //
    // set up destination address
    //
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(group);
    addr.sin_port = htons(port);

    //
    // now just sendto() our destination
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

        constexpr int delay_secs = 1;

        sleep(delay_secs); // Unix sleep is seconds
    }

    return 0;
}
