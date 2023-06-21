#include "udp_helper.h"

#include <time.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("Usage: %s '<server_addr>' '<server_port>' 'message' '[interface]' optional\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* const server_name = argv[1];
    const unsigned short server_port = (unsigned short) atoi(argv[2]);
    const char* const data_to_send = argv[3];
    const char* const source_iface = (argc == 5) ? argv[4] : nullptr;

    auto const opt = create_udp_client(source_iface);
    if (!opt)
    {
        return 1;
    }

    auto const sock = opt.value();

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_name);
    server_address.sin_port = htons(server_port);

    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    long const start_micro = tp.tv_sec * 1000 * 1000 * 1000 + tp.tv_nsec;

    // send data
    ssize_t const len = sendto(sock, data_to_send, strlen(data_to_send), 0,
            (struct sockaddr*) &server_address, sizeof(server_address));

    if (len < 0)
    {
        printf("sendto return zero");
        return 1;
    }

    size_t const clen = (size_t)(len);

    // received echoed data back
    char buffer[4096];
    recvfrom(sock, buffer, clen, 0, NULL, NULL);

    clock_gettime(CLOCK_REALTIME, &tp);
    long const end_micro = tp.tv_sec * 1000 * 1000 * 1000 + tp.tv_nsec;
    long const delta = end_micro - start_micro;

    buffer[len] = '\0';
    printf("time delta: %ld nanoseconds, recieved: '%s'\n", delta, buffer);

    return 0;
}
