#include <boost/lexical_cast.hpp>
#include <iostream>

#include "client.h"

void check_argc(int argc, char *program_name) {
    if (argc < 4 || argc > 5) {
        std::cerr << "USAGE: "
                  << program_name << " timestamp character host [port]"
                  << std::endl;
        exit(1);
    }
}

uint64_t get_timestamp(char *timestamp_str) {
    uint64_t timestamp;
    try {
        timestamp = boost::lexical_cast<uint64_t>(timestamp_str);
    } catch (std::exception e) {
        std::cerr << "Timestamp has to be a number." << std::endl;
        exit(1);
    }

    if (timestamp >= 71728934400) {
        std::cerr << "Timestamp has to be at least 0 and come from a year before 4243"
                  << std::endl;
        exit(1);
    }

    return timestamp;
}

in_port_t get_port(char *port_str) {
    uint16_t port;
    try {
        port = boost::lexical_cast<uint16_t>(port_str);
    } catch (std::exception e) {
        std::cerr << "Port has to be a number." << std::endl;
        exit(1);
    }

    return port;
}

Client parse_args(int argc, char *argv[]) {
    check_argc(argc, argv[0]);
    uint64_t timestamp = get_timestamp(argv[1]);
    char character = argv[2][0];
    std::string host = argv[3];
    in_port_t port = 20160;

    if (argc == 5) {
        port = get_port(argv[4]);
    }

    Address server_address(host, htons(port));

    return Client(timestamp, character, server_address);

}

int main(int argc, char *argv[]) {
    Client client = parse_args(argc, argv);

    client.send();

    while (1) {
        try {
            std::pair<ServerMessage, Address> received = client.receive();
            ServerMessage message = received.first;
            std::cout << message.client_message.timestamp << " "
                      << message.client_message.character << " "
                      << message.contents << std::endl;
        } catch (InvalidServerMessage e) {
            std::cerr << "Invalid message sent from "
                << e.culprit_address.to_string() << std::endl;
        }

    }

}
