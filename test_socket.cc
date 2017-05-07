#include <string>

#include <netdb.h>
#include <netinet/in.h>

#include "testing.h"
#include "socket.h"
#include "message.h"

void test_constructor() {
    begin_test();

    check_no_exception_thrown([] { Socket s(2222); }, "With argument.");
    check_no_exception_thrown([] { Socket s(2222); },
            "Socket closed on destruction.");
    check_exception_thrown<AddressNotAvailable>([] {
            Socket s1(2222);
            Socket s2(2222);
    }, "Can't open two sockets on the same port.");
    check_no_exception_thrown([] { Socket s; }, "Without argument.");
}

void test_client_message_communication() {
    begin_test();

    Socket s1(2222);
    Socket s2(2223);

    Address destination("localhost", htons(2223));
    ClientMessage to_send(0x42, 'c');
    s1.send(to_send, destination);

    std::pair<ClientMessage, Address> received = s2.receiveFromClient();
    ClientMessage message = received.first;
    Address address = received.second;

    ClientMessage to_send2(0x420, 'd');
    s2.send(to_send2, address);
    std::pair<ClientMessage, Address> received2 = s1.receiveFromClient();

    ClientMessage message2 = received2.first;
    Address address2 = received2.second;

    check_equal<uint64_t>(message.timestamp, 0x42,
            "Got the correct timestamp.");
    check_equal<char>(message.character, 'c',
            "Got the correct character.");
    check_equal<unsigned>(ntohs(address.port), 2222,
            "Got the correct sender port.");
    check_equal<uint64_t>(message2.timestamp, 0x420,
            "Got the correct timestamp.");
    check_equal<char>(message2.character, 'd',
            "Got the correct character.");
    check_equal<unsigned>(ntohs(address2.port), 2223,
            "Got the correct sender port.");
}

void test_server_message_communication() {
    begin_test();
}

int main() {
    test_constructor();
    test_client_message_communication();
}
