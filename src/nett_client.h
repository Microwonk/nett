#pragma once

#include <nett_common.h>
#include <nett_ts_queue.h>
#include <nett_message.h>
#include <nett_connection.h>

namespace nett {
    template <typename T>
    class client_if {

    public:
        client_if() : m_socket(m_context) {}

        virtual ~client_if() {
            disconnect();
        }

        bool connect(const std::string& host, const uint16_t port) {

            try {
                // resolve physical address
                asio::ip::tcp::resolver resolver(m_context);
                asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

                m_connection = std::make_unique<connection<T>>(
                    connection<T>::owner::client,
                    m_context,
                    asio::ip::tcp::socket(m_context),
                    m_q_messages_in);

                m_connection->connect_to_server(endpoints);

                thr_context = std::thread([this] { m_context.run(); });

            } catch (std::exception& ex) {
                fmt::println(stderr, "Client Exception: {}", ex.what());
                return false;
            }

            return true;
        }

        void disconnect() {
            if (is_connected()) {
                m_connection->disconnect();
            }

            m_context.stop();

            if (thr_context.joinable()) {
                thr_context.join();
            }

            m_connection.release();
        }

        bool is_connected() {
            if (m_connection) {
                return m_connection->is_connected();
            }
            return false;
        }

        void send(const message<T>& msg) {
            if (is_connected()) {
                m_connection->send(msg);
            }
        }

        auto incoming() -> ts_queue<owned_message<T>>& {
            return m_q_messages_in;
        }

    protected:
        // handles data transfer
        asio::io_context m_context;
        std::thread thr_context;
        asio::ip::tcp::socket m_socket;
        std::unique_ptr<connection<T>> m_connection;

    private:
        ts_queue<owned_message<T>> m_q_messages_in;
    };
} // namespace nett