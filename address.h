#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

class CouldNotResolveHostname : std::exception {};

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
        if (getaddrinfo(host.c_str(), NULL, &addr_hints, &addr_result) != 0) {
            throw CouldNotResolveHostname();
        }

        address =
            ((struct sockaddr_in*) (addr_result->ai_addr))->sin_addr.s_addr;

        freeaddrinfo(addr_result);
    }

    bool operator!=(const Address& other) const {
        return address != other.address || port != other.port;
    }

    bool operator<(const Address& other) const {
        return address < other.address || port < other.port;
    }

    struct sockaddr_in create_sockaddr_struct() {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = port;
        addr.sin_addr.s_addr = address;

        return addr;
    }

    std::string to_string() {
        uint8_t split[4];
        memcpy(split, &address, 4);

        char buffer[32];
        sprintf(buffer, "%u.%u.%u.%u:%u",
                split[0], split[1], split[2], split[3], port);

        std::string str(buffer);

        return str;
    }
};

#endif
