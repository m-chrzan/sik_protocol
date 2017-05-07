#ifndef ADDRESS_H
#define ADDRESS_H

#include <string.h>

struct Address {
    /* address and port in network (big-endian) byte order */
    in_addr_t address;
    in_port_t port;

    Address() {}

    Address(in_addr_t address, in_port_t port) : address(address), port(port) {}

    Address(in_port_t port) : Address(INADDR_ANY, port) {}

    Address(std::string host, in_port_t port) : port(port) {
        struct addrinfo addr_hints;
        struct addrinfo *addr_result;
        memset(&addr_hints, 0, sizeof(struct addrinfo));
        addr_hints.ai_family = AF_INET;
        addr_hints.ai_socktype = SOCK_DGRAM;
        addr_hints.ai_protocol = IPPROTO_UDP;
        addr_hints.ai_flags = 0;
        addr_hints.ai_addrlen = 0;
        addr_hints.ai_addr = NULL;
        addr_hints.ai_canonname = NULL;
        addr_hints.ai_next = NULL;
        getaddrinfo(host.c_str(), NULL, &addr_hints, &addr_result);

        address =
            ((struct sockaddr_in*) (addr_result->ai_addr))->sin_addr.s_addr;

        freeaddrinfo(addr_result);
    }

    struct sockaddr_in create_sockaddr_struct() {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = port;
        addr.sin_addr.s_addr = address;

        return addr;
    }
};

#endif
