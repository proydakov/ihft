#pragma once

#include <optional>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

std::optional<int> create_multicast_sender(const char* const source_iface)
{
    //
    // create what looks like an ordinary UDP socket
    //
    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return std::nullopt;
    }

    //
    // setup sender interface
    //
    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_interface.s_addr = source_iface ? inet_addr(source_iface) : htonl(INADDR_ANY);

    if (
        setsockopt(
            fd, IPPROTO_IP, IP_MULTICAST_IF, (void*) &mreq, sizeof(mreq)
        ) < 0
    ) {
        perror("setsockopt");
        return std::nullopt;
    }

    return fd;
}

std::optional<int> create_multicast_listener(const char* const group, const unsigned short port, const char* const source_iface)
{
    //
    // create what looks like an ordinary UDP socket
    //
    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return std::nullopt;
    }

    //
    // allow multiple sockets to use the same PORT number
    //
    const u_int yes = 1;
    if (
        setsockopt(
            fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)
        ) < 0
    ) {
       perror("Reusing ADDR failed");
       return std::nullopt;
    }

    //
    // set up destination address
    //
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(group);
    addr.sin_port = htons((unsigned short)port);

    //
    // bind to receive address
    //
    if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return std::nullopt;
    }

    //
    // use setsockopt() to request that the kernel join a multicast group
    //
    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = source_iface ? inet_addr(source_iface) : htonl(INADDR_ANY);
    if (
        setsockopt(
            fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)
        ) < 0
    ) {
        perror("setsockopt");
        return std::nullopt;
    }

    return fd;
}
