add_executable(client main.c)

target_link_libraries(client cargs)
if (WIN32)
    target_link_libraries(client ws2_32)
endif()