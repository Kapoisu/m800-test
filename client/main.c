#include <cargs.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #undef max
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

#define RETRY_INTERVAL_BASE 500 // in milliseconds
#define RETRY_INTERVAL_MULTIPLIER 2
#define RETRY_INTERVAL_MAX 8000

enum option_t {
    opt_help,
    opt_ip,
    opt_port,
    opt_retry
};

void print_manual()
{
    printf("Usage: client [-h | --help] [--ip=<address>] [--port=<number>] [--max-retry=<number>] <message>\n");
    printf("\nA simple UDP client.\n");
    printf("\nOptions:\n");
    printf("    --ip         The IP address of the server you want to connect.\n");
    printf("    --port       The port number of the targeted service.\n");
    printf("    --max-retry  The maxinum number of attempts to retry.\n");
    printf("\nExample:\n");
    printf("    ./client --ip=127.0.0.1 --port=1024\n");
}

unsigned int max(unsigned int value, unsigned int max_value)
{
    return value > max_value ? max_value : value;
}

void sleep_ms(unsigned int milliseconds)
{
#ifdef WIN32
    Sleep(milliseconds);
#else
    struct timespec duration;
    duration.tv_sec = milliseconds / 1000;
    duration.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&duration, NULL);
#endif
}

int main(int argc, char *argv[])
{
    cag_option_context context;
    struct cag_option options[] = {
        {.identifier = opt_help, .access_letters = "h", .access_name = "help"},
        {.identifier = opt_ip, .access_letters = NULL, .access_name = "ip", .value_name = "address"},
        {.identifier = opt_port, .access_letters = NULL, .access_name = "port", .value_name = "number"},
        {.identifier = opt_retry, .access_letters = NULL, .access_name = "max-retry", .value_name = "number"}
    };

    cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);

    struct sockaddr_in server = {.sin_family = AF_INET};
    const char* ip_address = DEFAULT_IP_ADDRESS;
    int port_number = DEFAULT_PORT_NUMBER;
    int max_retry = 0;
    bool has_ip = false;
    bool has_port = false;
    char message[MESSAGE_SIZE_MAX + 1] = {0};

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
            case opt_retry:
                value = cag_option_get_value(&context);
                if (value != NULL) {
                    max_retry = atoi(value);
                }
                break;
            default:
                break;
        }
    }

    strncpy(message, argv[context.index], MESSAGE_SIZE_MAX);

    printf("Start message sender.\n");

    if (!has_ip) {
        printf("\nIP address is not specified, use %s instead.\n", DEFAULT_IP_ADDRESS);
    }

    if (!has_port) {
        printf("\nPort number is not specified, use %d instead.\n", DEFAULT_PORT_NUMBER);
    }

    if (inet_pton(AF_INET, ip_address, &server.sin_addr) < 1) {
        printf("\nIP address: invalid argument\n");
        return EXIT_FAILURE;
    }
    server.sin_port = htons((uint16_t)port_number);

    file_descriptor socket_fd = socket(AF_INET, SOCK_DGRAM, PF_UNSPEC);
    if (socket_fd < 0) {
        perror("Create socket");
        return EXIT_FAILURE;
    }

    if (connect(socket_fd, (const struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connect");
        return EXIT_FAILURE;
    }

    printf("\nConnect to [%s:%d]\n", ip_address, port_number);

    int retry = 0;
    unsigned int wait_interval = RETRY_INTERVAL_BASE;
    bool can_retry = false;

    do {
        if (sendto(socket_fd, message, (socklen_t)strlen(message), 0, (const struct sockaddr*)&server, sizeof(server)) < 0) {
            perror("Send message");
        }
        else if (recvfrom(socket_fd, message, MESSAGE_SIZE_MAX, 0, NULL, NULL) < 0) {
            perror("Receive message");
        }
        else {
            printf("Echo received: %s\n", message);
            break;
        }

        can_retry = retry < max_retry;
        if (can_retry) {
            printf("Waiting to retry...\n");
            wait_interval *= RETRY_INTERVAL_MULTIPLIER;
            sleep_ms(max(wait_interval, RETRY_INTERVAL_MAX));
            ++retry;
        }
        else {
            return EXIT_FAILURE;
        }
    } while (can_retry);

    return 0;
}