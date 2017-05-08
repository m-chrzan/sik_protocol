#include <iostream>

#include "server.h"
#include "socket.h"

int main() {
    Server server(21060, "test");

    while (1) {
        for (Action action : server.allowedActions()) {
            if (action == RECEIVE) {
                server.receive();
            } else if (server.have_pending_messages() && action == SEND) {
                server.send();
            }
        }
    }

    return 0;
}
