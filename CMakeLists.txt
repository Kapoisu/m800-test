cmake_minimum_required(VERSION 3.22)
project(m800-test)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS "-Wpedantic -Wall -Wconversion")

#add_subdirectory(client)
add_subdirectory(server)