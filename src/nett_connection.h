#pragma once

#include <nett_common.h>
#include <nett_ts_queue.h>
#include <nett_message.h>

namespace nett {

    template <typename T>
    class connection : public std::enable_shared_from_this<connection<T>> {
    public:

        enum class owner
        {
            server,
            client
        };

        connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, ts_queue<owned_message<T>>& qIn)
                : m_socket(std::move(socket)), m_asio_context(asioContext), m_q_messages_in(qIn)
        {
            m_owner_type = parent;
        }


        virtual ~connection() {

        }

        [[nodiscard]] uint32_t get_id() const {
            return id;
        }

        void connect_to_server(const asio::ip::tcp::resolver::results_type& endpoints) {
            if (m_owner_type == owner::client) {
                asio::async_connect(m_socket, endpoints,
                    [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
                        if (!ec) {
                            read_header();
                        }
                    });
            }
        };

        void connect_to_client(const uint32_t uid = 0) {
            if (m_owner_type == owner::server) {
                if (m_socket.is_open()) {
                    id = uid;
                    read_header();
                }
            }
        }

        void disconnect() {
            if (is_connected()) {
                asio::post(m_asio_context, [this] { m_socket.close(); });
            }
        };

        [[nodiscard]] bool is_connected() const {
            return m_socket.is_open();
        };

        void send(const message<T>& msg) {
            asio::post(m_asio_context,
                [this, msg] {
                    const bool writingMessage = !m_q_messages_out.empty();
                    m_q_messages_out.push_back(msg);
                    if (!writingMessage) {
                        write_header();
                    }
                });
        }

        void read_header() {
            asio::async_read(m_socket, asio::buffer(&m_msg_temp_in.header, sizeof(message_header<T>)),
                [this](std::error_code ec, std::size_t length) {
                    if (!ec) {
                        if (m_msg_temp_in.header.size > 0) {
                            m_msg_temp_in.body.resize(m_msg_temp_in.header.size);
                            read_body();
                        } else {
                            add_to_incoming_msg_q();
                        }
                    } else {
                        fmt::println("[{}] Read Header Fail.", id);
                        m_socket.close();
                    }
                });
        }

        void read_body() {
            asio::async_read(m_socket, asio::buffer(m_msg_temp_in.body.data(), m_msg_temp_in.body.size()),
                [this] (std::error_code ec, std::size_t length) {
                    if (!ec) {
                        add_to_incoming_msg_q();
                    } else {
                        fmt::println("[{}] Read Body Fail.", id);
                        m_socket.close();
                    }
                });
        }

        void write_header() {
            asio::async_write(m_socket, asio::buffer(&m_q_messages_out.front().header, sizeof(message_header<T>)),
                [this] (std::error_code ec, std::size_t length) {
                    if (!ec) {
                        if (m_q_messages_out.front().body.size() > 0) {
                            write_body();
                        } else {
                            m_q_messages_out.pop_front();
                            if (!m_q_messages_out.empty()) {
                                write_header();
                            }
                        }
                    } else {
                        fmt::println("[{}] Write Header Fail.", id);
                        m_socket.close();
                    }
                });
        }

        void write_body() {
            asio::async_write(m_socket, asio::buffer(m_q_messages_out.front().body.data(), m_q_messages_out.front().body.size()),
                [this] (std::error_code ec, size_t length) {
                    if (!ec) {
                        m_q_messages_out.pop_front();
                        if (!m_q_messages_out.empty()) {
                            write_header();
                        }
                    } else {
                        fmt::println("[{}] Write Body Fail.", id);
                        m_socket.close();
                    }
                });
        }

        void add_to_incoming_msg_q() {
            if (m_owner_type == owner::server) {
                m_q_messages_in.push_back({ this->shared_from_this(), m_msg_temp_in });
            } else {
                m_q_messages_in.push_back({ nullptr, m_msg_temp_in });
            }

            // another task for the asio async
            read_header();
        }

    protected:
        // each connection has a unique socket to a remote
        asio::ip::tcp::socket m_socket;

        // shared context
        asio::io_context& m_asio_context;

        // holds all messages that are to be sent to remote
        ts_queue<message<T>> m_q_messages_out;

        // messages received
        // is a reference, because it should be provided by server or client
        ts_queue<owned_message<T>>& m_q_messages_in;

        message<T> m_msg_temp_in;

        owner m_owner_type = owner::server;

        uint32_t id = 0;
    };

} // namespace nett