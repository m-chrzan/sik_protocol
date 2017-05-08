#include <iostream>

#include <boost/lexical_cast.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

#include "server.h"
#include "socket.h"

void check_argc(int argc, char *program_name) {
    if (argc != 3) {
        std::cerr << "USAGE: "
                  << program_name << " port filename"
                  << std::endl;
        exit(1);
    }
}

in_port_t get_port(char *port_str) {
    return boost::lexical_cast<uint16_t>(port_str);
}

std::string get_contents(char *filename) {
    int file = open(filename, O_RDONLY);

    char buffer[64 * 1024];

    read(file, buffer, 64 * 1024);

    std::string str(buffer);

    return str;
}

Server parse_args(int argc, char *argv[]) {
    check_argc(argc, argv[0]);

    in_port_t port = get_port(argv[1]);

    std::string contents = get_contents(argv[2]);

    return Server(port, contents);
}

int main(int argc, char *argv[]) {
    Server server = parse_args(argc, argv);

    while (1) {
        for (Action action : server.allowed_actions()) {
            try {
                switch (action) {
                    case RECEIVE:
                        server.receive();
                        break;
                    case SEND:
                        server.send();
                        break;
                }
            } catch (InvalidClientMessage e) {
                std::cerr << "Invalid message sent from "
                    << e.culprit_address.to_string() << std::endl;
            } catch (WouldBlock e) {
            }
        }

        server.clear_inactive_clients();
    }

    return 0;
}
