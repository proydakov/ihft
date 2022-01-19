#pragma once

#include <optional>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>

std::optional<int> create_udp_server(const unsigned short server_port, const char* const source_iface)
{
    // open socket
    int const sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("could not create socket\n");
        return std::nullopt;
    }

    // socket address used for the server
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((unsigned short)server_port);
    server_address.sin_addr.s_addr = source_iface ? inet_addr(source_iface) : htonl(INADDR_ANY);

    // bind it to listen to the incoming connections on the created server address
    if ((bind(sock, (struct sockaddr*) &server_address, sizeof(server_address))) < 0) {
        printf("could not bind socket\n");
        return std::nullopt;
    }

    return sock;
}

std::optional<int> create_udp_client(const char* const source_iface)
{
    // open socket
    int const sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("could not create socket\n");
        return std::nullopt;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = source_iface ? inet_addr(source_iface) : htonl(INADDR_ANY);

    // bind to send address
    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return std::nullopt;
    }

    return sock;
}
