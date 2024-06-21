#include <iostream>

#include <fmt/core.h>

#define PROJECT_NAME "nett"

int main(int argc, char **argv) {
    if(argc != 1) {
        fmt::println("{} take no argument", argv[0]);
        return 1;
    }
    fmt::println("This is project {}", PROJECT_NAME);
    return 0;
}
