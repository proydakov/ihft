#pragma once

#include "arg_helper.h"

#include <optional>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

namespace ihft::network
{

inline std::optional<int> create_multicast_sender(const char* const source_iface)
{
    in_addr source_address{};
    source_address.s_addr = htonl(INADDR_ANY);
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

    //
    // create what looks like an ordinary UDP socket
    //
    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        std::perror("socket");
        return std::nullopt;
    }

    //
    // setup sender interface
    //
    // IP_MULTICAST_IF expects an interface address (struct in_addr), not the
    // ip_mreq used by IP_ADD_MEMBERSHIP.
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &source_address, sizeof(source_address)) < 0)
    {
        std::perror("setsockopt");
        close(fd);
        return std::nullopt;
    }

    return fd;
}

inline std::optional<int> create_multicast_listener(const char* const group, const unsigned short port, const char* const source_iface)
{
    auto const group_address = parse_ipv4(group);
    if (not group_address)
    {
        std::printf("invalid multicast group: %s\n", group);
        return std::nullopt;
    }

    in_addr source_address{};
    source_address.s_addr = htonl(INADDR_ANY);
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

    //
    // create what looks like an ordinary UDP socket
    //
    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        std::perror("socket");
        return std::nullopt;
    }

    //
    // allow multiple sockets to use the same PORT number
    //
    int const yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
       std::perror("Reusing ADDR failed");
       close(fd);
       return std::nullopt;
    }

    //
    // set up destination address
    //
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr = *group_address;
    addr.sin_port = htons(port);

    //
    // bind to receive address
    //
    if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        std::perror("bind");
        close(fd);
        return std::nullopt;
    }

    //
    // use setsockopt() to request that the kernel join a multicast group
    //
    struct ip_mreq mreq;
    std::memset(&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr = *group_address;
    mreq.imr_interface = source_address;
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        std::perror("setsockopt");
        close(fd);
        return std::nullopt;
    }

    return fd;
}

} // namespace ihft::network
