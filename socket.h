#ifndef SOCKET_H
#define SOCKET_H

#include <exception>

#include <sys/socket.h>
#include <unistd.h>

#include "address.h"
#include "message.h"

class AddressNotAvailable : std::exception {};

class Socket {
public:
    Socket() {
        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd_ == -1) {
            throw errno;
        }
    }

    Socket(in_port_t port) : Socket() {
        address_ = Address(htons(port));

        struct sockaddr_in my_addr = address_.create_sockaddr_struct();
        int rv = bind(fd_, (struct sockaddr *) &my_addr,
                (socklen_t) sizeof(my_addr));

        if (rv < 0) {
            if (errno == EALREADY || errno == EADDRINUSE ||
                    errno == EADDRNOTAVAIL) {
                throw AddressNotAvailable();
            }
        }
    }

    ~Socket() {
        close(fd_);
    }

    void send(ClientMessage &message, Address address) {
        uint8_t buffer[9];
        message.to_bytes(buffer);

        struct sockaddr_in dest_addr = address.create_sockaddr_struct();

        sendto(fd_, buffer, 9, 0, (struct sockaddr *)&dest_addr,
                sizeof(dest_addr));
    }

    void send(ServerMessage &message, Address address) {
        uint8_t buffer[64 * 1024];
        size_t length = message.to_bytes(buffer);

        struct sockaddr_in dest_addr = address.create_sockaddr_struct();

        sendto(fd_, buffer, length, 0, (struct sockaddr *)&dest_addr,
                sizeof(dest_addr));
    }

    std::pair<ClientMessage, Address> receiveFromClient() {
        struct sockaddr_in client_addr;
        socklen_t addr_length = sizeof(client_addr);
        uint8_t buffer[9];
        recvfrom(fd_, buffer, 9, 0, (struct sockaddr *)&client_addr,
                &addr_length);

        in_addr_t address = client_addr.sin_addr.s_addr;
        in_port_t port = client_addr.sin_port;
        return std::make_pair(ClientMessage(buffer), Address(address, port));
    }

    std::pair<ServerMessage, Address> receiveFromServer() {
        struct sockaddr_in server_addr;
        socklen_t addr_length = sizeof(server_addr);
        uint8_t buffer[64 * 1024];
        recvfrom(fd_, buffer, 64 * 1024, 0, (sockaddr *) &server_addr,
                &addr_length);

        in_addr_t address = server_addr.sin_addr.s_addr;
        in_port_t port = server_addr.sin_port;
        return std::make_pair(ServerMessage(buffer), Address(address, port));
    }

private:
    int fd_;
    Address address_;
};

#endif
