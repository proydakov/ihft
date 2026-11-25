// https://tldp.org/HOWTO/Multicast-HOWTO-6.html
//
// Example:
// ./network_multicast_sender 239.255.255.251 27335 192.168.88.50
//

#include "arg_helper.h"
#include "multicast_helper.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
       std::printf("Command line args should be multicast group and port\n");
       std::printf("(e.g. for SSDP, `sender 239.255.255.250 1900 [interface_ip]`)\n");
       return EXIT_FAILURE;
    }

    const char* const group = argv[1]; // e.g. 239.255.255.250 for SSDP
    auto const port = ihft::network::parse_port(argv[2]);
    if (not port)
    {
        std::printf("invalid port: %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    const char* const source_iface = (argc == 4 ? argv[3] : nullptr);

    auto const group_address = ihft::network::parse_ipv4(group);
    if (not group_address)
    {
        std::printf("invalid multicast group: %s\n", group);
        return EXIT_FAILURE;
    }

    auto const opt = ihft::network::create_multicast_sender(source_iface);
    if (!opt)
    {
        return EXIT_FAILURE;
    }

    const int fd = opt.value();

    //
    // set up destination address
    //
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr = *group_address;
    addr.sin_port = htons(*port);

    //
    // now just sendto() our destination
    //
    for(unsigned i = 0; ; i++)
    {
        char buffer[64];
        std::memset(buffer, '\0', sizeof(buffer));
        std::snprintf(buffer, sizeof(buffer), "Hello, World! Sequence: %u", i & 0xFF);

        ssize_t const nbytes = sendto(
            fd,
            buffer,
            sizeof(buffer),
            0,
            (struct sockaddr*) &addr,
            sizeof(addr)
        );
        if (nbytes < 0)
        {
            std::perror("sendto");
            return EXIT_FAILURE;
        }

        constexpr int delay_secs = 1;

        sleep(delay_secs); // Unix sleep is seconds
    }

    return EXIT_SUCCESS;
}
