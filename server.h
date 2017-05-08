#ifndef SERVER_H
#define SERVER_H

#include <iostream>

#include <map>
#include <vector>

#include "socket.h"
#include "bufferedmessage.h"

class Server {
public:
    Server(in_port_t port, std::string contents) :
        socket_(port), contents_(contents), buffer_(4096) {}

    std::vector<Action> allowedActions() {
        return socket_.allowedActions();
    }

    void receive() {
        time_t current_time = time(NULL);

        std::pair<ClientMessage, Address> received =
            socket_.receiveFromClient();

        validate_(received.first, received.second);

        std::vector<Address> recipients =
            find_different_active_clients_(current_time, received.second);

        if (!recipients.empty()) {
            buffer_.add_message(received.first, recipients);
        }

        active_clients_[received.second] = current_time;
    }

    void send() {
        std::pair<ClientMessage, Address> next_message = buffer_.get_next();

        ServerMessage full_message(next_message.first, contents_);

        socket_.send(full_message, next_message.second);

        buffer_.pop_next();
    }

    bool have_pending_messages() {
        return buffer_.have_pending_messages();
    }

    void clear_inactive_clients() {
        time_t current_time = time(NULL);

        std::vector<Address> stale_addresses;

        for (auto client : active_clients_) {
            if (current_time - client.second > 2 * 60) {
                stale_addresses.push_back(client.first);
            }
        }

        for (Address stale_address : stale_addresses) {
            active_clients_.erase(stale_address);
        }
    }

private:
    Socket socket_;
    std::string contents_;
    std::map<Address, time_t> active_clients_;
    MessageBuffer buffer_;

    std::vector<Address> find_different_active_clients_(time_t time,
            Address address) {
        std::vector<Address> active_clients;
        for (auto client : active_clients_) {
            if (client.first != address && time - client.second < 2 * 60) {
                active_clients.push_back(client.first);
            }
        }

        return active_clients;
    }

    void validate_(ClientMessage message, Address address) {
        if (message.timestamp >= 71728934400) {
            throw  InvalidClientMessage(address);
        }
    }
};

#endif
