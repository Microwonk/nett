cmake_minimum_required(VERSION 3.21)

project(nett VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

# find all required packages
find_package(fmt CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)

add_subdirectory(src)
add_subdirectory(client)
add_subdirectory(server)