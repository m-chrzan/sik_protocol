# SIK Protocol

Implementation of a client and server for a sample network communication
protocol.

## Running the server

    make server
    ./server port file

Where `port` is an available port number and `file` is a file that is smaller
than 63KB.

## Running the client

    make client
    ./client timestamp char host [port]

Where `timestamp` is a decimal Unix timestamp, `char` is a single character,
`host` is the server's hostname, and `port` is the server's port number (20160
by default).

## The protocol

Valid datagrams are of the form `<timestamp><character>[<string>]`.

* `<timestamp>` is a 64-bit unsigned integer representing a Unix timestamp in a
year no later than 4242.

* `<character>` is a single ASCII character.

* `<string>` is an optional string of ASCII characters.
