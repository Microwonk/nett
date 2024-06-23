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
                    [this](std::error_code ec, asio::ip::tcp::endpoint) {
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

        void send(const message<T>& msg);

        void read_header() {

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