#include <iostream>

#include <fmt/core.h>
#include "TcpServer.h"

#define PROJECT_NAME "nett"

auto main() -> int {
    fmt::println("This is project {}", PROJECT_NAME);

    try {
        run_tcp_server(8080);
    } catch (std::exception& e) {
        fmt::println("ERROR: {}", e.what());
    }
}
