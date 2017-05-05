#include "testing.h"
#include "socket.h"

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

int main() {
    test_constructor();
}
