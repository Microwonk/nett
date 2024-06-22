#pragma once
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class TcpServer {
public:
    TcpServer(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
    }

    auto run() -> void;

private:
    auto start_accept() -> void;

    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(tcp::socket socket) : socket_(std::move(socket)) {}

        inline auto start() -> void {
            do_read();
        }

    private:
        auto do_read() -> void;

        auto do_write(std::size_t length) -> void;

        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
    };

    tcp::acceptor acceptor_;
};

inline auto run_tcp_server(short port) -> void {
    boost::asio::io_context io_context;
        TcpServer server(io_context, 8080);
        io_context.run();
}
