#pragma once
#include <cstdint>
#include <memory>

#include "nett_connection.h"

namespace nett {
    template <typename T>
    class server_if {

    public:
        explicit server_if(const uint16_t port) : m_asio_acceptor(m_asio_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)){

        }

        virtual ~server_if() {
            stop();
        }

        bool start() {
            try {
                wait_for_client_connection();

                m_thread_context = std::thread([this] { m_asio_context.run(); });
            } catch (std::exception& ex) {
                fmt::println(stderr, "[SERVER] Exception: {}", ex.what());
                return false;
            }
            fmt::println("[SERVER] Started");
            return true;
        }

        void stop() {
            m_asio_context.stop();

            if (m_thread_context.joinable()) m_thread_context.join();

            fmt::println("[SERVER] Stopped");
        }

        // Async
        void wait_for_client_connection() {
            m_asio_acceptor.async_accept(
                [this](std::error_code ec, asio::ip::tcp::socket socket) {
                    if (!ec) {
                        fmt::println("[SERVER] New Connection: {}", socket.remote_endpoint().address().to_string());

                        // if (std::shared_ptr<connection<T>> newConn =
                        //         std::make_shared<connection<T>>(connection<T>::owner::server, m_asio_context, std::move(socket), m_q_messages_in); on_client_connect(newConn)) {
                        //     m_deq_connections.push_back(std::move(newConn));
                        //
                        //     m_deq_connections.back()->connect_to_client(n_id_counter++);
                        //
                        //     fmt::println("[{}] Connection Approved", m_deq_connections.back()->get_id());
                        //
                        // } else {
                        //     fmt::println("[SERVER] Connection Denied");
                        // }

                    } else {
                        fmt::println(stderr, "[-----] New Connection Error: {}", ec.message());
                    }

                    wait_for_client_connection();
                });
        }

        void message_client(std::shared_ptr<connection<T>> client, const message<T>& msg) {
            if (client && client->is_connected()) {
                client->send(msg);
            } else {
                on_client_disconnect(client);
                client.reset();
                m_deq_connections.erase(
                    std::remove(m_deq_connections.begin(), m_deq_connections.end(), client), m_deq_connections.end());
            }
        }

        void broadcast_to_clients(const message<T>& msg, std::shared_ptr<connection<T>> ignoreClient = nullptr) {

            bool invalidClientExists = false;

            for (auto& client : m_deq_connections) {
                if (client && client->is_connected()) {
                    if (client != ignoreClient) {
                        client->send(msg);
                    }
                } else {
                    on_client_disconnect(client);
                    client.reset();
                    invalidClientExists = true;
                }
            }

            if (invalidClientExists) {
                m_deq_connections.erase(
                    std::remove(m_deq_connections.begin(), m_deq_connections.end(), nullptr), m_deq_connections.end());
            }
        }

        void update(const size_t nMaxMessages = -1) {
            size_t nMessageCount = 0;
            while (nMessageCount < nMaxMessages && !m_q_messages_in.empty()) {
                auto msg = m_q_messages_in.pop_front();

                on_message(msg.remote, msg.msg);

                nMessageCount++;
            }
        }

    protected:

        virtual bool on_client_connect(std::shared_ptr<connection<T>> client) {
            return false;
        }

        virtual void on_client_disconnect(std::shared_ptr<connection<T>> client) {

        }

        virtual void on_message(std::shared_ptr<connection<T>> client, message<T>& msg) {

        }

        ts_queue<owned_message<T>> m_q_messages_in;

        std::deque<std::shared_ptr<connection<T>>> m_deq_connections;

        asio::io_context m_asio_context;
        std::thread m_thread_context;

        asio::ip::tcp::acceptor m_asio_acceptor;

        uint32_t n_id_counter = 10000;

    };
} // namespace nett
