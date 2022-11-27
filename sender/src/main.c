#include "error.h"
#include "keypad.h"
#include "lcd.h"
#include "option_handler.h"
#include "rudp_v2.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE (100)
#define DEFAULT_LIFE (5)
#define FIVE_SECONDS (5000)
#define THREE_SECONDS (3000)
#define MAX_LCD_LENGTH (16)

void get_user_num(char *user_num);

int main(int argc, char *argv[]) {
    int result;
    struct options opts;
    options_init(&opts);

    // parse command line arguments
    result = parse_arguments(argc, argv, &opts);
    if (result == FAIL) {
        fprintf(stderr, "Usage: %s -o <server ip address> [-p server port_number]\n", argv[0]);         // NOLINT(cert-err33-c)
        exit(EXIT_FAILURE);                                                                             // NOLINT(concurrency-mt-unsafe)
    }

    // init wiringPi
    if (wiringPiSetup() == -1) { //when initialize wiring failed,print message to screen
        printf("setup wiringPi failed !");
        exit(EXIT_FAILURE);                                                                             // NOLINT(concurrency-mt-unsafe)
    }

    // init lcd, keypad
    {
        init_lcd();
        keypadInit();
    }

    // open a socket
    opts.sock_fd = socket(AF_INET, SOCK_DGRAM, 0);       // NOLINT(android-cloexec-socket)
    if (opts.sock_fd == -1) {
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] open a socket", EXIT_FAILURE);
    }

	printf("%s\n", opts.ip_out);

    // init server addr
    struct sockaddr_in to_addr;
    struct sockaddr_in from_addr;
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(opts.port_out);
    to_addr.sin_addr.s_addr = inet_addr(opts.ip_out);
    if (to_addr.sin_addr.s_addr == (in_addr_t) -1) {
        close(opts.sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] initiate proxy server's sockaddr_in", EXIT_FAILURE);
    }
    
	// start ui, connect server
	lcd_write(0, 0, "Welcome to Guess");
    lcd_write(0, 1, "Wait server...");
    char buffer[BUFFER_SIZE] = {0, };
    strcpy(buffer, "CONNECT");
    result = rudp_send(opts.sock_fd, &to_addr, buffer, strlen(buffer), RUDP_SYN);
    if (result != -0) {
        close(opts.sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] rudp_send", EXIT_FAILURE);
    }
    lcd_write(0, 14, "OK");
    delay(THREE_SECONDS);

    lcd_clear();
    lcd_write(0, 0, "Welcome to Guess");
    lcd_write(1, 1, "Guess 0 to 9999");
    delay(FIVE_SECONDS);
    lcd_clear();

    int life = DEFAULT_LIFE;
    while (life > 0) {
        char user_num[5] = {'\0'};
        lcd_clear();

        char msg[MAX_LCD_LENGTH] = { 0 };
        sprintf(msg, "Life: %d, LOW", life);
        lcd_write(0, 0, msg);
        lcd_write(0, 1, "number:");
        get_user_num(user_num);

        // for test, TODO delete this
        strcpy(buffer, user_num);
        buffer[4] = '\n';
        buffer[5] = '\0';
        printf("%s\n", buffer);

        result = rudp_send(opts.sock_fd, &to_addr, buffer, strlen(buffer), RUDP_SYN);
        if (result != 0) {
            close(opts.sock_fd);
            fatal_message(__FILE__, __func__, __LINE__, "[FAIL] rudp_send", EXIT_FAILURE);
        }

        char data_from_server[BUFFER_SIZE];
        rudp_recv(opts.sock_fd, data_from_server, &from_addr);
        printf("%s\n", data_from_server);

        life--;
    }

    result = rudp_send(opts.sock_fd, &to_addr, buffer, BUFFER_SIZE, RUDP_FIN);
    lcd_clear();
    lcd_write(0, 0, "Finish!");
    lcd_write(1, 1, "Good job!");
    close(opts.sock_fd);
    return EXIT_SUCCESS;
}

void get_user_num(char *user_num) {
    // store user input (4 digits)
    unsigned char pressed_keys[BUTTON_NUM];
    unsigned char last_key_pressed[BUTTON_NUM];
    int count = 0;
    int loc = 9;
    char user_input[2];

    lcd_write(0, 1, "Number: ");
    // get user input
    while (1) {
        keyRead(pressed_keys);
        bool comp = keyCompare(pressed_keys, last_key_pressed);
        if (!comp) {
            user_input[0] = getKey(pressed_keys);
            user_input[1] = '\0';
            if ('0' <= user_input[0] && user_input[0] <= '9') {
                user_num[count++] = user_input[0];
                lcd_write(loc++, 1, user_input);
            }
            keyCopy(last_key_pressed, pressed_keys);
        }
        delay(100);
        if (count == 4) {
            break;
        }
    }
}
