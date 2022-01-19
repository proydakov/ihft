#include "udp_helper.h"

#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: %s 'port' '[interface]' optional\n", argv[0]);
        return 1;
    }

    const unsigned short server_port = (unsigned short) atoi(argv[1]);
    const char* const source_iface = (argc == 3) ? argv[2] : nullptr;

    auto const opt = create_udp_server(server_port, source_iface);
    if (!opt)
    {
        return 1;
    }

    auto const sock = opt.value();

    // socket address used to store client address
    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address));
    socklen_t client_address_len = sizeof(client_address);

    // run indefinitely
    while (true) {
        char buffer[4096];

        // read content into buffer from an incoming client
        ssize_t const len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_address, &client_address_len);

        if (len < 0)
        {
            continue;
        }

        size_t const clen = (size_t)(len);

        // send same content back to the client ("echo")
        sendto(sock, buffer, clen, 0, (struct sockaddr*)&client_address, sizeof(client_address));

        buffer[len] = '\0';
        // inet_ntoa prints user friendly representation of the ip address
        printf("received: '%s' from client %s:%d\n", buffer, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    }

    return 0;
}
