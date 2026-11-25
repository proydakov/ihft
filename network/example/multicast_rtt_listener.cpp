// https://tldp.org/HOWTO/Multicast-HOWTO-6.html
//
// Example:
// ./network_multicast_listener 239.255.255.251 27335 192.168.88.50
//

#include "arg_helper.h"
#include "multicast_helper.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::printf("Command line args should be multicast group and port and [interface] optional\n");
        std::printf("(e.g. for SSDP, `listener 239.255.255.250 1900 [192.168.1.1]`)\n");
        return EXIT_FAILURE;
    }

    const char* const group = argv[1]; // e.g. 239.255.255.250 for SSDP
    auto const port = ihft::network::parse_port(argv[2]);
    if (not port)
    {
        std::printf("invalid port: %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    const char* const source_iface = (argc == 4) ? argv[3] : nullptr;

    auto const opt = ihft::network::create_multicast_listener(group, *port, source_iface);
    if (!opt)
    {
        return EXIT_FAILURE;
    }

    const int fd = opt.value();

    //
    // now just enter a read-print loop
    //
    struct sockaddr_in addr{};
    char msgbuf[4096];
    while(true)
    {
        unsigned addrlen = sizeof(addr);
        ssize_t const nbytes = recvfrom(
            fd,
            msgbuf,
            sizeof(msgbuf),
            0,
            (struct sockaddr*) &addr,
            &addrlen
        );
        if (nbytes < 0)
        {
            std::perror("recvfrom");
            return EXIT_FAILURE;
        }
        struct timespec tp;
        ::clock_gettime(CLOCK_REALTIME, &tp);
        long const end_micro = tp.tv_sec * 1000 * 1000 * 1000 + tp.tv_nsec;

        long start_micro = 0;
        if (nbytes == sizeof(start_micro))
        {
            long const* start_micro_ptr = (long const*) msgbuf;
            start_micro = *start_micro_ptr;
            long const delta = end_micro - start_micro;

            std::printf("from: %s time delta: %ld nanoseconds\n", inet_ntoa(addr.sin_addr), delta);
        }
     }

    return EXIT_SUCCESS;
}
