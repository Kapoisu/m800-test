cmake_minimum_required(VERSION 3.14)
project(m800-test)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(cargs URL https://github.com/likle/cargs/archive/refs/tags/v1.0.3.zip)
FetchContent_MakeAvailable(cargs)

add_subdirectory(client)
add_subdirectory(server)