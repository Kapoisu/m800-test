add_executable(client main.c)

target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/lib/cargs/include)
target_link_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/lib/cargs)
target_link_libraries(client cargs)
if (WIN32)
    target_link_libraries(client ws2_32)
endif()