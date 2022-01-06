#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if (argc < 4) {
        printf("Usage: %s '<server_addr>' '<server_port>' 'message' '[interface]' optional\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* server_name = argv[1];
    const int server_port = atoi(argv[2]);
    const char* data_to_send = argv[3];
    const char* source_iface = (argc == 5) ? argv[4] : NULL;

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_name);
    server_address.sin_port = htons((unsigned short)server_port);

    // open socket
    int const sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("could not create socket\n");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = source_iface ? inet_addr(source_iface) : htonl(INADDR_ANY);

    // bind to send address
    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    long const start_micro = tp.tv_sec * 1000 * 1000 * 1000 + tp.tv_nsec;

    // send data
    ssize_t const len = sendto(sock, data_to_send, strlen(data_to_send), 0,
            (struct sockaddr*) &server_address, sizeof(server_address));

    if (len < 0) {
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

    // close the socket
    close(sock);

    return 0;
}

