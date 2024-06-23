#pragma once

#include <nett_common.h>

namespace nett {

    template <typename T>
    struct message_header {
        T id{};
        uint32_t size = 0;
    };

    template <typename T>
    struct message {
        message_header<T> header{};
        std::vector<uint8_t> body;

        [[nodiscard]] auto size() const -> size_t {
            return sizeof(message_header<T>) + body.size();
        }

        friend std::ostream& operator << (std::ostream& os, const message& msg) {
            os << "ID:" << static_cast<int>(msg.header.id) << " Size:" << msg.header.size;
            return os;
        }

        template<typename DataType>
        friend message& operator << (message& msg, const DataType& data) {
            // check if the pushed data can be copied
            static_assert(std::is_standard_layout_v<DataType>, "Data is too complex to be pushed into vector");

            // cache current size of vector
            size_t i = msg.body.size();

            // resize by the size of bushed data
            msg.body.resize(msg.body.size() + sizeof(DataType));

            // copy data into the allocated space
            std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

            msg.header.size = msg.size();
            return msg;
        }

        template<typename DataType>
        friend message& operator >> (message& msg, DataType& data) {
            // check if the pushed data can be copied
            static_assert(std::is_standard_layout_v<DataType>, "Data is too complex to be pushed into vector");

            // cache location towards end of vector
            size_t i = msg.body.size() - sizeof(DataType);

            // copy data from vec into variable
            std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

            // shrink
            msg.body.resize(i);

            msg.header.size = msg.size();
            return msg;
        }
    };

    template <typename T>
    class connection;

    template <typename T>
    struct owned_message {
        std::shared_ptr<connection<T>> remote = nullptr;
        message<T> msg;

        friend std::ostream& operator << (std::ostream& os, const owned_message& msg) {
            os << msg.msg;
            return os;
        }
    };
} // namespace nett