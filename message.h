#ifndef MESSAGE_H
#define MESSAGE_H

#include <endian.h>
#include <netinet/in.h>
#include <string.h>
#include <string>

#include "address.h"

struct Message {
    virtual int to_bytes(uint8_t *buffer) = 0;
};

struct ClientMessage : Message {
    uint64_t timestamp;
    char character;

    ClientMessage(uint64_t timestamp, char character) :
        timestamp(timestamp),
        character(character) {}

    ClientMessage(uint8_t *bytes) :
        timestamp(be64toh(*((uint64_t *) bytes))),
        character(((char *) bytes)[8]) {}

    int to_bytes(uint8_t *buffer) {
        int offset = 0;

        uint64_t big_endian_timestamp = htobe64(timestamp);
        memcpy(buffer + offset, &big_endian_timestamp, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        memcpy(buffer + offset, &character, sizeof(char));
        offset += sizeof(char);

        return offset;
    }
};

struct ServerMessage : Message {
    ClientMessage client_message;
    std::string contents;

    ServerMessage(uint64_t timestamp, char character, std::string contents) :
        client_message(timestamp, character), contents(contents) {}

    ServerMessage(uint8_t *bytes) :
        client_message(bytes),
        contents((char *)(bytes+9)) {}

    int to_bytes(uint8_t *buffer) {
        int offset = client_message.to_bytes(buffer);

        memcpy(buffer + offset, contents.c_str(), contents.length());
        offset += contents.length();

        buffer[offset] = 0;
        offset += 1;

        return offset;
    }
};

#endif
