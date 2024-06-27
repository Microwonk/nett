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
        switch (msg.header.id)
        {
            case ServerMessages::ServerPing:
            {
                fmt::println("[{}]: Server Ping", client->get_id());

                // Simply bounce message back to client
                client->send(msg);
            }
            break;

            case ServerMessages::MessageAll:
            {
                fmt::println("[{}]: Message All", client->get_id());

                // Construct a new message and send it to all clients
                nett::message<ServerMessages> msg;
                msg.header.id = ServerMessages::ServerMessage;
                msg << client->get_id();
                broadcast_to_clients(msg, client);
            }
            break;
        }
    }
};

[[noreturn]] auto main() -> int {
    Server server(60000);
    server.start();

    while (true) {
        server.update();
    }
}
