#include <cargs.h>
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

enum option_t {
    opt_help,
    opt_ip,
    opt_port
};

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

void print_error(const char* event)
{
#ifdef _WIN32
    printf("%s: %d\n", event, WSAGetLastError());
#else
    perror(event);
#endif
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(2, &wsa_data);
#endif
    cag_option_context context;
    struct cag_option options[] = {
        {.identifier = opt_help, .access_letters = "h", .access_name = "help"},
        {.identifier = opt_ip, .access_letters = NULL, .access_name = "ip", .value_name = "address"},
        {.identifier = opt_port, .access_letters = NULL, .access_name = "port", .value_name = "number"},
    };

    cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);

    struct sockaddr_in server = {.sin_family = AF_INET};
    const char* ip_address = DEFAULT_IP_ADDRESS;
    int port_number = DEFAULT_PORT_NUMBER;
    bool has_ip = false;
    bool has_port = false;

    while (cag_option_fetch(&context)) {
        const char *value = NULL;
        switch (cag_option_get(&context)) {
            case opt_help:
                if (argc == 2) {
                    print_manual();
                    return 0;
                }
            case opt_ip:
                value = cag_option_get_value(&context);
                if (value != NULL) {
                    ip_address = value;
                    has_ip = true;
                }
                break;
            case opt_port:
                value = cag_option_get_value(&context);
                if (value != NULL) {
                    port_number = atoi(value);
                    has_port = true;
                }
                break;
            default:
                break;
        }
    }

    printf("Start echo server.\n");

    if (!has_ip) {
        printf("\nIP address is not specified, use %s instead.\n", DEFAULT_IP_ADDRESS);
    }

    if (!has_port) {
        printf("\nPort number is not specified, use %d instead.\n", DEFAULT_PORT_NUMBER);
    }

    if (inet_pton(AF_INET, ip_address, &server.sin_addr) < 1) {
        printf("IP address: invalid argument\n");
        return EXIT_FAILURE;
    }
    server.sin_port = htons((uint16_t)port_number);

    file_descriptor socket_fd = socket(AF_INET, SOCK_DGRAM, PF_UNSPEC);
    if (socket_fd < 0) {
        print_error("socket()");
        return EXIT_FAILURE;
    }

    if (bind(socket_fd, (const struct sockaddr*)&server, sizeof(server)) < 0) {
        print_error("bind()");
        return EXIT_FAILURE;
    }

    printf("\nListen on [%s:%d]\n", ip_address, port_number);

    struct sockaddr_in client;
    socklen_t address_length = sizeof(client);
    char message[MESSAGE_SIZE_MAX + 1] = {0};
    char sender[ADDRESS_SIZE_MAX + 1] = {0};

    while (true) {
        if (recvfrom(socket_fd, message, MESSAGE_SIZE_MAX, 0, (struct sockaddr*)&client, &address_length) < 0) {
            print_error("recvfrom()");
            continue;
        }

        if (inet_ntop(AF_INET, &client.sin_addr, sender, ADDRESS_SIZE_MAX) != 0) {
            printf("Message received from [%s:%d]: %s\n", sender, ntohs(client.sin_port), message);
        }

        if (sendto(socket_fd, message, (socklen_t)strlen(message), 0, (const struct sockaddr*)&client, address_length) < 0) {
            print_error("sendto()");
            continue;
        }
    }

    return 0;
}