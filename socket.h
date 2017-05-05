#ifndef SOCKET_H
#define SOCKET_H

#include <exception>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

class AddressNotAvailable : std::exception {};

class Socket {
public:
    Socket() {
        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd_ == -1) {
            throw errno;
        }
    }

    Socket(int port) : Socket() {
        address_ = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = {
                .s_addr = htonl(INADDR_ANY)
            }
        };

        int rv = bind(fd_, (struct sockaddr *) &address_,
                (socklen_t) sizeof(address_));

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

private:
    int fd_;
    struct sockaddr_in address_;
};

#endif
