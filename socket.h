#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>

#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <errno.h>

#include "address.h"
#include "message.h"

class CouldNotCreateSocket : std::exception {};
class AddressNotAvailable : std::exception {};
class MessageTooLarge : std::exception {};
class WouldBlock : std::exception {};
class InvalidClientMessage : std::exception {
public:
    Address culprit_address;

    InvalidClientMessage(Address culprit_address) : 
        culprit_address(culprit_address) {}
};

enum Action { SEND, RECEIVE };

class Socket {
public:
    Socket() {
        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd_ == -1) {
            throw CouldNotCreateSocket();
        }
    }

    Socket(in_port_t port) {
        fd_ = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
        if (fd_ == -1) {
            throw CouldNotCreateSocket();
        }

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

    std::vector<Action> allowed_actions(short events) {
        struct pollfd fds[1];
        fds[0] = { .fd = fd_, .events = events };
        poll(fds, 1, -1);

        std::vector<Action> actions;

        if (fds[0].revents & POLLIN) {
            actions.push_back(RECEIVE);
        }

        if (fds[0].revents & POLLOUT) {
            actions.push_back(SEND);
        }

        return actions;
    }


    void send(ClientMessage &message, Address address) {
        send_(9, message, address);
    }

    void send(ServerMessage &message, Address address) {
        send_(64 * 1024, message, address);
    }

    std::pair<ClientMessage, Address> receiveFromClient() {
        struct sockaddr_in client_addr;
        socklen_t addr_length = sizeof(client_addr);
        uint8_t buffer[10];

        int rv = recvfrom(fd_, buffer, 10, 0, (struct sockaddr *)&client_addr,
                &addr_length);

        if (rv == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                throw WouldBlock();
            }
        }

        Address address(client_addr.sin_addr.s_addr, client_addr.sin_port);
        
        if (rv != 9) {
            throw InvalidClientMessage(address);
        }

        return std::make_pair(ClientMessage(buffer), address);
    }

    std::pair<ServerMessage, Address> receiveFromServer() {
        struct sockaddr_in server_addr;
        socklen_t addr_length = sizeof(server_addr);
        uint8_t buffer[64 * 1024];

        int rv = recvfrom(fd_, buffer, 64 * 1024, 0, (sockaddr *) &server_addr,
                &addr_length);

        if (rv == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                throw WouldBlock();
            }
        }

        in_addr_t address = server_addr.sin_addr.s_addr;
        in_port_t port = server_addr.sin_port;
        return std::make_pair(ServerMessage(buffer), Address(address, port));
    }

private:
    int fd_;
    Address address_;

    void send_(int buffer_size, Message& message, Address address) {
        uint8_t buffer[buffer_size];
        size_t length = message.to_bytes(buffer);

        struct sockaddr_in dest_addr = address.create_sockaddr_struct();

        int rv = sendto(fd_, buffer, length, 0, (struct sockaddr *)&dest_addr,
                sizeof(dest_addr));

        if (rv == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                throw WouldBlock();
            } else if (errno == EMSGSIZE) {
                throw MessageTooLarge();
            }
        }
    }
};

#endif
