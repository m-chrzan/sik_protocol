#ifndef CLIENT_H
#define CLIENT_H

#include "socket.h"
#include "address.h"

class Client {
public:
    Client(uint64_t timestamp, char character, Address server_address) :
        timestamp_(timestamp), character_(character),
        server_address_(server_address) {}

    void send() {
        ClientMessage message(timestamp_, character_);
        socket_.send(message, server_address_);
    }

    std::pair<ServerMessage, Address> receive() {
        return socket_.receiveFromServer();
    }

private:
    uint64_t timestamp_;
    char character_;
    Address server_address_;
    Socket socket_;
};

#endif
