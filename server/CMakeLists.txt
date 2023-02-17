add_executable(server main.c)

target_link_libraries(server cargs)
if (WIN32)
    target_link_libraries(server ws2_32)
endif()