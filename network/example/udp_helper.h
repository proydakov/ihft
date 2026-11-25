#pragma once

#include "arg_helper.h"

#include <optional>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

namespace ihft::network
{

inline std::optional<int> create_udp_server(const unsigned short server_port, const char* const source_iface)
{
    in_addr source_address{};
    if (source_iface != nullptr)
    {
        auto const parsed = parse_ipv4(source_iface);
        if (not parsed)
        {
            std::printf("invalid source interface: %s\n", source_iface);
            return std::nullopt;
        }
        source_address = *parsed;
    }
    else
    {
        source_address.s_addr = htonl(INADDR_ANY);
    }

    // open socket
    int const sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::printf("could not create socket\n");
        return std::nullopt;
    }

    // socket address used for the server
    struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr = source_address;

    // bind it to listen to the incoming connections on the created server address
    if ((bind(sock, (struct sockaddr*) &server_address, sizeof(server_address))) < 0)
    {
        std::printf("could not bind socket\n");
        close(sock);
        return std::nullopt;
    }

    return sock;
}

inline std::optional<int> create_udp_client(const char* const source_iface)
{
    in_addr source_address{};
    if (source_iface != nullptr)
    {
        auto const parsed = parse_ipv4(source_iface);
        if (not parsed)
        {
            std::printf("invalid source interface: %s\n", source_iface);
            return std::nullopt;
        }
        source_address = *parsed;
    }
    else
    {
        source_address.s_addr = htonl(INADDR_ANY);
    }

    // open socket
    int const sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::printf("could not create socket\n");
        return std::nullopt;
    }

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr = source_address;

    // bind to send address
    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        std::perror("bind");
        close(sock);
        return std::nullopt;
    }

    return sock;
}

} // namespace ihft::network
