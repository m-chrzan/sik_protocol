CXX=g++
CXXFLAGS=-Wall -g -O2

SRCS=server.cc client.cc
TESTS=test_socket.cc

test_socket: test_socket.cc socket.h
	$(CXX) $(CXXFLAGS) -o $@ $<

tests: test_socket

.PHONY: tests run_tests clean

clean:
	rm -f *.o $(SRCS:.cc=) $(TESTS:.cc=)
