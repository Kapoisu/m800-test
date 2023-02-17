#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET file_descriptor;
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    typedef int file_descriptor;
#endif

#define DEFAULT_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT_NUMBER 1024
#define MESSAGE_SIZE_MAX 1024
#define ADDRESS_SIZE_MAX 15

enum argument {
    index_ip = 1,
    index_port = 2
};

bool has_ip_address(int argc)
{
    return argc > index_ip ? true : false;
}

bool has_port_number(int argc)
{
    return argc > index_port ? true : false;
}

void print_manual()
{
    printf("Usage: server [-h | --help] [--ip=<address>] [--port=<number>]\n");
    printf("\nA simple UDP echo server.\n");
    printf("\nOptions:\n");
    printf("    --ip    The IP address of the host.\n");
    printf("    --port  The port number you want to listen to.\n");
    printf("\nExample:\n");
    printf("    ./server --ip=127.0.0.1 --port=1024\n");
    printf("\nNote:\n");
    printf("    On some operating systems, you might need root access to use privileged port numbers.\n");
}

int main(int argc, char *argv[])
{
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_manual();
        return 0;
    }

    struct sockaddr_in server = {.sin_family = AF_INET};
    const char* ip_address = DEFAULT_IP_ADDRESS;
    int port_number = DEFAULT_PORT_NUMBER;

    printf("Start echo server.\n");

    if (has_ip_address(argc)) {
        printf("IP address: %s\n", argv[index_ip]);
        ip_address = argv[index_ip];
    }
    else {
        printf("IP address is not specified, use %s instead.\n", DEFAULT_IP_ADDRESS);
    }

    if (inet_pton(AF_INET, ip_address, &server.sin_addr) == 0) {
        perror("IP address");
    }

    if (has_port_number(argc)) {
        printf("Port: %s\n", argv[index_port]);
        port_number = atoi(argv[index_port]);
    }
    else {
        printf("Port number is not specified, use %d instead.\n", DEFAULT_PORT_NUMBER);
    }

    server.sin_port = htons((uint16_t)port_number);

    file_descriptor socket_fd = socket(AF_INET, SOCK_DGRAM, PF_UNSPEC);
    if (socket_fd < 0) {
        perror("Create socket");
        return EXIT_FAILURE;
    }

    if (bind(socket_fd, (const struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Bind socket");
        return EXIT_FAILURE;
    }

    printf("Listen on [%s:%d]\n", ip_address, port_number);

    struct sockaddr_in client;
    socklen_t address_length;
    char message[MESSAGE_SIZE_MAX + 1];
    message[MESSAGE_SIZE_MAX] = 0;
    char sender[ADDRESS_SIZE_MAX + 1];
    sender[ADDRESS_SIZE_MAX] = 0;

    while (true) {
        if (recvfrom(socket_fd, message, MESSAGE_SIZE_MAX, 0, (struct sockaddr*)&client, &address_length) < 0) {
            perror("Receive message");
            continue;
        }

        if (inet_ntop(AF_INET, &client.sin_addr, sender, ADDRESS_SIZE_MAX) != 0) {
            printf("Message received from [%s:%d]: %s\n", sender, ntohs(client.sin_port), message);
        }

        if (sendto(socket_fd, message, (socklen_t)strlen(message), 0, (const struct sockaddr*)&client, address_length) < 0) {
            perror("Send message");
            continue;
        }
    }

    return 0;
}