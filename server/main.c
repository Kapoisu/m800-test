#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define DEFAULT_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT_NUMBER 1024
#define BUFFER_SIZE 1024

enum argument {
    index_ip = 1,
    index_port = 2
};

typedef int file_descriptor;

bool has_ip_address(int argc)
{
    return argc > index_ip ? true : false;
}

bool has_port_number(int argc)
{
    return argc > index_port ? true : false;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server = {.sin_family = AF_INET, .sin_port = 0, .sin_addr = {0}};
    const char* ip_address = DEFAULT_IP_ADDRESS;
    int port_number = DEFAULT_PORT_NUMBER;

    printf("Start Echo Server.\n");
    for (int i = 0; i < argc; ++i) {
        printf("%s\n", argv[i]);
    }

    if (has_ip_address(argc)) {
        printf("IP address: %s\n", argv[index_ip]);
        ip_address = argv[index_ip];
    }
    else {
        printf("IP address is not specified, use %s instead.\n", ip_address);
    }

    if (inet_pton(AF_INET, ip_address, &server.sin_addr) == 0) {
        perror("The IP address is invalid.\n");
    }

    if (has_port_number(argc)) {
        printf("Port: %s\n", argv[index_port]);
        port_number = atoi(argv[index_port]);
    }
    else {
        printf("Port number is not specified, use %i instead.\n", port_number);
    }

    server.sin_port = htons((uint16_t)port_number);

    file_descriptor socket_fd = socket(AF_INET, SOCK_DGRAM, PF_UNSPEC);
    if (socket_fd < 0) {
        perror("Failed to open a socket.\n");
        return 1;
    }

    if (bind(socket_fd, (const struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Failed to bind the socket.\n");
        return 1;
    }

    printf("Listen on %s:%d\n", ip_address, port_number);

    struct sockaddr_in client;
    socklen_t client_len;
    char message[BUFFER_SIZE];
    while (true) {
        ssize_t bytes = recvfrom(socket_fd, message, BUFFER_SIZE, 0, (struct sockaddr*)&client, &client_len);
        if (bytes < 0) {
            perror("Failed to receive the message.\n");
            continue;
        }

        printf("%.*s\n", (int)bytes, message);
        bytes = sendto(socket_fd, message, (size_t)bytes, 0, (const struct sockaddr*)&client, client_len);

        if (bytes < 0) {
            perror("Failed to send the message.\n");
            continue;
        }
    }

    return 0;
}