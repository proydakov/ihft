#include "arg_helper.h"
#include "udp_helper.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::printf("Usage: %s '<server_addr>' '<server_port>' 'message' '[interface]' optional\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* const server_name = argv[1];
    auto const server_port = ihft::network::parse_port(argv[2]);
    if (not server_port)
    {
        std::printf("invalid port: %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    const char* const data_to_send = argv[3];
    const char* const source_iface = (argc == 5) ? argv[4] : nullptr;

    auto const server_address = ihft::network::parse_ipv4(server_name);
    if (not server_address)
    {
        std::printf("invalid server address: %s\n", server_name);
        return EXIT_FAILURE;
    }

    auto const opt = ihft::network::create_udp_client(source_iface);
    if (!opt)
    {
        return EXIT_FAILURE;
    }

    auto const sock = opt.value();

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr = *server_address;
    addr.sin_port = htons(*server_port);

    struct timespec tp;
    ::clock_gettime(CLOCK_REALTIME, &tp);
    long const start_micro = tp.tv_sec * 1000 * 1000 * 1000 + tp.tv_nsec;

    // send data
    ssize_t const len = sendto(sock, data_to_send, std::strlen(data_to_send), 0,
            (struct sockaddr*) &addr, sizeof(addr));

    if (len < 0)
    {
        std::perror("sendto");
        return EXIT_FAILURE;
    }

    // received echoed data back
    char buffer[4096];
    ssize_t const received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
    if (received < 0)
    {
        std::perror("recvfrom");
        return EXIT_FAILURE;
    }

    ::clock_gettime(CLOCK_REALTIME, &tp);
    long const end_micro = tp.tv_sec * 1000 * 1000 * 1000 + tp.tv_nsec;
    long const delta = end_micro - start_micro;

    buffer[static_cast<size_t>(received)] = '\0';
    std::printf("time delta: %ld nanoseconds, received: '%s'\n", delta, buffer);

    return EXIT_SUCCESS;
}
