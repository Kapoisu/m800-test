add_executable(server main.c)

target_compile_options(server PRIVATE -Wpedantic -Wall -Wconversion)
target_link_libraries(server cargs)
if (WIN32)
    target_link_libraries(server ws2_32)
endif()