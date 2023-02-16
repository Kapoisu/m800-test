add_executable(server main.c)
if (WIN32)
    target_link_libraries(server ws2_32)
endif()