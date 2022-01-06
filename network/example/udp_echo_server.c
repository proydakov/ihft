#include <arpa/inet.h>
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
	if (argc < 2) {
        printf("Usage: %s 'port' '[interface]' optional\n", argv[0]);
        return 1;
    }

    const int server_port = atoi(argv[1]);
    const char* source_iface = (argc == 3) ? argv[2] : NULL;

    // socket address used for the server
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((unsigned short)server_port);
    server_address.sin_addr.s_addr = source_iface ? inet_addr(source_iface) : htonl(INADDR_ANY);

    int const sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("could not create socket\n");
        return 1;
    }

    // bind it to listen to the incoming connections on the created server
    // address, will return -1 on error
    if ((bind(sock, (struct sockaddr*) &server_address, sizeof(server_address))) < 0) {
        printf("could not bind socket\n");
        return 1;
    }

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
