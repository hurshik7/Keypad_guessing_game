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

void key_print_lcd(unsigned char *pressed_key, int *count, char *user_num, int* loc);
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
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(opts.port_out);
    to_addr.sin_addr.s_addr = inet_addr(opts.ip_out);
    if (to_addr.sin_addr.s_addr == (in_addr_t) -1) {
        close(opts.sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] initiate proxy server's sockaddr_in", EXIT_FAILURE);
    }
    
	// start ui here
	lcd_write(0, 0, "Welcome to Guess");

    char user_num[5] = {'\0'};
	get_user_num(user_num);

    // for test, TODO delete this
    char buffer[BUFFER_SIZE] = {0, };
    strcpy(buffer, user_num);
    buffer[4] = '\n';
    buffer[5] = '\0';
    printf("%s\n", buffer);

    result = rudp_send(opts.sock_fd, &to_addr, buffer, strlen(buffer), RUDP_SYN);
    if (result != -0) {
        close(opts.sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] rudp_send", EXIT_FAILURE);
    }
    lcd_write(7, 1, "... OK");

    close(opts.sock_fd);
    return EXIT_SUCCESS;
}

void key_print_lcd(unsigned char *pressed_key, int *count, char *user_num, int* loc) {
    char user_input[2] = { 0, };
    user_input[0] = getKey(pressed_key);
    user_input[1] = '\0';
    if ('0' <= user_input[0] && user_input[0] <= '9') {
        user_num[*count] = user_input[0];
        lcd_write(*loc, 1, user_input);
        *count += 1;
        loc++;
    }
}

void get_user_num(char *user_num) {
    // store user input (4 digits)
    unsigned char pressed_keys[BUTTON_NUM];
    unsigned char last_key_pressed[BUTTON_NUM];
    int count = 0;
    int loc = 8;
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

    // print user number on the lcd
    lcd_clear();
    lcd_write(0, 0, "Your number is..");
    lcd_write(2, 1, user_num);
}
