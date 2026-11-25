#include "arg_helper.h"
#include "udp_helper.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::printf("Usage: %s 'port' '[interface]' optional\n", argv[0]);
        return EXIT_FAILURE;
    }

    auto const server_port = ihft::network::parse_port(argv[1]);
    if (not server_port)
    {
        std::printf("invalid port: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    const char* const source_iface = (argc == 3) ? argv[2] : nullptr;

    auto const opt = ihft::network::create_udp_server(*server_port, source_iface);
    if (!opt)
    {
        return EXIT_FAILURE;
    }

    auto const sock = opt.value();

    // socket address used to store client address
    struct sockaddr_in client_address;
    std::memset(&client_address, 0, sizeof(client_address));

    // run indefinitely
    while(true)
    {
        char buffer[4096];

        // read content into buffer from an incoming client
        socklen_t client_address_len = sizeof(client_address);
        ssize_t const len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_address, &client_address_len);

        if (len < 0)
        {
            continue;
        }

        size_t const clen = static_cast<size_t>(len);

        // send same content back to the client ("echo")
        ssize_t const sent = sendto(sock, buffer, clen, 0, (struct sockaddr*)&client_address, sizeof(client_address));
        if (sent < 0)
        {
            std::perror("sendto");
            continue;
        }

        // recvfrom may fill the whole buffer, so keep the terminator in bounds.
        size_t const term = (clen < sizeof(buffer)) ? clen : (sizeof(buffer) - 1);
        buffer[term] = '\0';
        // inet_ntoa prints user friendly representation of the ip address
        std::printf("received: '%s' from client %s:%d\n", buffer, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    }

    return EXIT_SUCCESS;
}
