#include <nett.h>

#include "nett_client.h"

enum class ServerMessages : uint32_t {
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
};


class Client : public nett::client_if<ServerMessages> {
public:
    void ping_server()
    {
        nett::message<ServerMessages> msg;
        msg.header.id = ServerMessages::ServerPing;

        // Caution with this...
        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

        msg << timeNow;
        send(msg);
    }

    void message_all()
    {
        nett::message<ServerMessages> msg;
        msg.header.id = ServerMessages::MessageAll;
        send(msg);
    }
};


auto main() -> int {
    Client c;
    c.connect("127.0.0.1", 60000);

    if (c.is_connected())
    {

        c.ping_server();
        c.message_all();
        while (true) {
            if (!c.incoming().empty())
            {
                switch (auto msg = c.incoming().pop_front().msg; msg.header.id)
                {
                    case ServerMessages::ServerAccept:
                    {
                        // Server has responded to a ping request
                        fmt::println("Server Accepted Connection");
                    }
                    break;


                    case ServerMessages::ServerPing:
                    {
                        // Server has responded to a ping request
                        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                        std::chrono::system_clock::time_point timeThen;
                        msg >> timeThen;
                        fmt::println("Ping: {}", std::chrono::duration<double>(timeNow - timeThen).count());
                    }
                    break;

                    case ServerMessages::ServerMessage:
                    {
                        // Server has responded to a ping request
                        uint32_t clientID;
                        msg >> clientID;
                        fmt::println("Hello from [{}]", clientID);
                    }
                    break;
                }
            }
        }

    }
    else
    {
        fmt::println("Server Down");
    }

    return 0;
}