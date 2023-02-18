add_executable(client main.c)

target_compile_options(client PRIVATE -Wpedantic -Wall -Wconversion)
target_link_libraries(client cargs)
if (WIN32)
    target_link_libraries(client ws2_32)
endif()