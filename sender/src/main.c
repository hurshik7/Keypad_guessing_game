#include "error.h"
#include "keypad.h"
#include "lcd.h"
#include "rudp_v2.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_PORT (5050)
#define BUFFER_SIZE (10)

int count = 0;
int loc = 8;
char user_num[5] = {'\0'};
char user_input[2];

void key_print_lcd(unsigned char *a);

void key_print_lcd(unsigned char *a) {
    user_input[0] = getKey(a);
    user_input[1] = '\0';
    if ('0' <= user_input[0] && user_input[0] <= '9') {
        user_num[count++] = user_input[0];
        lcd_write(loc++, 1, user_input);
    }
}

int main(int argc, char *argv[]) {
    // store user input (4 digits)
    unsigned char pressed_keys[BUTTON_NUM];
    unsigned char last_key_pressed[BUTTON_NUM];

    while (1) {
        keyRead(pressed_keys);
        bool comp = keyCompare(pressed_keys, last_key_pressed);
        if (!comp) {
            key_print_lcd(pressed_keys);
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

    // send data to server
    int result;
    struct sockaddr_in to_addr;
    int sock_fd;

    // open a socket
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);       // NOLINT(android-cloexec-socket)
    if (sock_fd == -1) {
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] open a socket", EXIT_FAILURE);
    }

    // init server addr
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(DEFAULT_PORT);
    to_addr.sin_addr.s_addr = inet_addr(argv[1]);
    if (to_addr.sin_addr.s_addr == (in_addr_t) -1) {
        close(sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] initiate proxy server's sockaddr_in", EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE] = {0};
    strcpy(buffer, user_num);
    buffer[4] = '\n';
    buffer[5] = '\0';
    printf("%s\n", buffer);

    result = rudp_send(sock_fd, &to_addr, buffer, strlen(buffer), RUDP_SYN);
    if (result != -0) {
        close(sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] sendto", EXIT_FAILURE);
    }
    lcd_write(7, 1, "... OK");

    close(sock_fd);
    return EXIT_SUCCESS;
}

