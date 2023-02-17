add_executable(server main.c)

target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/lib/cargs/include)
target_link_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/lib/cargs)
target_link_libraries(server cargs)
if (WIN32)
    target_link_libraries(server ws2_32)
endif()