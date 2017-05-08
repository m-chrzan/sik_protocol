CXX=g++
CXXFLAGS=-Wall -g -O2 -std=c++14

SRCS=server.cc client.cc
TESTS=test_socket.cc

all: client test_socket server

server: server.cc server.h

client: client.cc client.h

test_socket: test_socket.cc socket.h
	$(CXX) $(CXXFLAGS) -o $@ $<

tests: test_socket

.PHONY: tests run_tests clean all

clean:
	rm -f *.o $(SRCS:.cc=) $(TESTS:.cc=)
