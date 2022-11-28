#include "error.h"
#include "lcd.h"
#include "option_handler.h"
#include "rudp_v2.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MAX_TARGET_NUM (10000)              // NOLINT(modernize-macro-to-enum)
#define DEFAULT_LIFE (5)                    // NOLINT(modernize-macro-to-enum)
#define BUF_LENGTH (128)                    // NOLINT(modernize-macro-to-enum)

void print_status_lcd(const char *status, int user_num);

int main(int argc, char *argv[]) {          // NOLINT(readability-function-cognitive-complexity)
    int result;
    struct options opts;
    options_init(&opts);

    // parse command line arguments
    result = parse_arguments(argc, argv, &opts);
    if (result == FAIL) {
        fprintf(stderr, "Usage: %s [-i ip_address] [-p port_number]\n", argv[0]);                       // NOLINT(cert-err33-c)
        exit(EXIT_FAILURE);                                                                             // NOLINT(concurrency-mt-unsafe)
    }

    // init wiringPi
    if (wiringPiSetup() == -1) { //when initialize wiring failed,print message to screen
        printf("setup wiringPi failed !");
        exit(EXIT_FAILURE);                                                                             // NOLINT(concurrency-mt-unsafe)
    }

    // init lcd
    {
        ledInit();
        init_lcd();
    }

    struct sockaddr_in addr;
    struct sockaddr_in from_addr;
    int option;

    // create a socket
    opts.sock_fd = socket(AF_INET, SOCK_DGRAM, 0);                                           // NOLINT(android-cloexec-socket)
    if(opts.sock_fd == -1)
    {
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] socket", EXIT_FAILURE);
    }

    // init addr
    addr.sin_family = AF_INET;
    addr.sin_port = htons(opts.port_in);
    addr.sin_addr.s_addr = opts.ip_in ? inet_addr(opts.ip_in) : htonl(INADDR_ANY);
    if(addr.sin_addr.s_addr == (in_addr_t) -1) // NOLINT(clang-analyzer-core.UndefinedBinaryOperatorResult)
    {
        close(opts.sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] initiate sockaddr_in", EXIT_FAILURE);
    }

    // set up the socket
    option = 1;
    setsockopt(opts.sock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // bind
    result = bind(opts.sock_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if(result == -1)
    {
        close(opts.sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] bind", EXIT_FAILURE);
    }

    srand(time(NULL));                                  // NOLINT(cert-msc32-c,cert-msc51-cpp)
    int life = DEFAULT_LIFE;
    printf("Listening port %d\n", opts.port_in);
    int random_num;

    lcd_write(0, 0, "Waiting");
    lcd_write(7, 1, "a client");
    while (1) {
        char data[MAX_DATA_LENGTH];
        char response_data[MAX_DATA_LENGTH];

        memset(data, '\0', MAX_DATA_LENGTH);
        result = rudp_recv(opts.sock_fd, data, &from_addr);
        if (strcmp(data, "CONNECT") == 0) {
            // init the game setting and start the game.
            printf("Client ip address: %s\n", inet_ntoa(from_addr.sin_addr));       // NOLINT(concurrency-mt-unsafe)
            life = DEFAULT_LIFE;
            memset(response_data, 0 , MAX_DATA_LENGTH);
            sprintf(response_data, "Guess/%d", life);
            rudp_send(opts.sock_fd, &from_addr, response_data, strlen(response_data), RUDP_SYN);
            random_num = rand() % MAX_TARGET_NUM;                                   // NOLINT(cert-msc30-c,cert-msc50-cpp,clang-analyzer-security.insecureAPI.rand,concurrency-mt-unsafe)
            printf("Random number: %d\n", random_num);
            lcd_clear();
            lcd_write(0, 0, "Game start!");
            lcd_write(9, 1, "waiting user num");
            continue;
        }
        if (result == RUDP_FIN) {
            // finish current game and get ready for other client.
            rudp_send(opts.sock_fd, &from_addr, response_data, strlen(response_data), RUDP_INIT);
            lcd_write(0, 0, "Game finish");
            lcd_write(0, 1, "Waiting a client");
            continue;
        }

        char *ptr;
        int client_num = (int) strtol(data, &ptr, 10);              // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        printf("user number: %d\n", client_num);

        memset(response_data, '\0', MAX_DATA_LENGTH);
        if (client_num > random_num) {                              // NOLINT(clang-analyzer-core.UndefinedBinaryOperatorResult)
            sprintf(response_data, "HIGH/%d", --life);
            print_status_lcd("HIGH", client_num);
        } else if (client_num < random_num) {
            sprintf(response_data, "LOW/%d", --life);
            print_status_lcd("HIGH", client_num);
        } else {
            sprintf(response_data, "CORRECT/%d", life);
            print_status_lcd("HIGH", client_num);
        }
        printf("%s\n", response_data);

        result = rudp_send(opts.sock_fd, &from_addr, response_data, strlen(response_data), RUDP_SYN);
        if (result != 0) {
            close(opts.sock_fd);
            fatal_message(__FILE__, __func__, __LINE__, "[FAIL] rudp_send", EXIT_FAILURE);
        }
    }

    close(opts.sock_fd);
    return EXIT_SUCCESS;
}

void print_status_lcd(const char *status, int user_num) {
    char first_line[BUF_LENGTH];
    char second_line[BUF_LENGTH];
    memset(first_line, 0, BUF_LENGTH);
    memset(second_line, 0, BUF_LENGTH);
    sprintf(first_line, "%s%s", "Status: ", status);
    sprintf(second_line, "%s%d", "Number: ", user_num);
    if (strcmp("CORRECT", status) == 0) {
        ledColorSet(0x00,0xff,0x00);
    } else {
        ledColorSet(0xff,0x00,0x00);
    }
    lcd_write(0, 0, first_line);
    lcd_write(0, 1, second_line);
}
