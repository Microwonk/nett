#include <nett.h>

#include "nett_server.h"

enum class ServerMessages : uint32_t {
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
};

class Server : public nett::server_if<ServerMessages> {
public:
    explicit Server(const uint16_t port) : server_if(port) {}

protected:
    bool on_client_connect(std::shared_ptr<nett::connection<ServerMessages>> client) override {
        return true;
    }

    void on_client_disconnect(std::shared_ptr<nett::connection<ServerMessages>> client) override {

    }

    void on_message(std::shared_ptr<nett::connection<ServerMessages>> client, nett::message<ServerMessages> &msg) override {

    }
};

[[noreturn]] auto main() -> int {
    Server server(60000);
    server.start();

    while (true) {
        server.update();
    }
}
