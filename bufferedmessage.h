#ifndef BUFFERED_MESSAGE_H
#define BUFFERED_MESSAGE_H

#include <deque>
#include <vector>

#include "address.h"
#include "message.h"

class MessageBuffer {
public:
    MessageBuffer(size_t size) : size_(size) {}

    void add_message(ClientMessage message, std::vector<Address> recipients) {
        if (buffer_.size() >= size_) {
            buffer_.pop_front();
        }

        buffer_.emplace_back(message, recipients);
    }

    std::pair<ClientMessage, Address> get_next() {
        BufferedMessage message = buffer_.front();
        return std::make_pair(message.message, message.recipients.back());
    }

    void pop_next() {
        if (!buffer_.empty()) {
            buffer_.front().recipients.pop_back();
            if (buffer_.front().recipients.empty()) {
                buffer_.pop_front();
            }
        }
    }

    bool have_pending_messages() {
        return !buffer_.empty();
    }

private:
    struct BufferedMessage {
        ClientMessage message;
        std::vector<Address> recipients;

        BufferedMessage(ClientMessage message, std::vector<Address> recipients)
            : message(message), recipients(recipients) {}
    };

    size_t size_;
    std::deque<BufferedMessage> buffer_;
};

#endif
