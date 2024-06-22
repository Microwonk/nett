#include "TcpServer.h"

auto TcpServer::start_accept() -> void {
    tcp::socket socket(acceptor_.get_executor().context());
        acceptor_.async_accept(socket, [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket))->start();
            }
            start_accept();
        });
}

auto TcpServer::run() -> void {
    acceptor_.get_executor().context();
}

auto TcpServer::Session::do_read() -> void {
    auto self(shared_from_this());
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                [this, self](boost::system::error_code ec, std::size_t length) {
                    if (!ec) {
                        do_write(length);
                    }
                });
}

auto TcpServer::Session::do_write(std::size_t length) -> void {
    auto self(shared_from_this());
            boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                    if (!ec) {
                        do_read();
                    }
                });
}
