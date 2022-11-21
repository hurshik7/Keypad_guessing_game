#include "error.h"
#include "keypad.h"
#include "lcd.h"
#include "udp_sender.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int count = 0;
int loc = 8;
char user_num[5] = { '\0' };
char user_input[2];

void key_print_lcd(unsigned char* a);

void key_print_lcd(unsigned char* a) {
	user_input[0] = getKey(a);
	user_input[1] = '\0';
	if ('0' <= user_input[0] && user_input[0] <= '9') {
		user_num[count++] = user_input[0];
		lcd_write(loc++, 1, user_input);
	}
}

/**
 * Runs rudp client.
 * @param argc the number of command line arguments
 * @param argv an array of string represents command line arguments
 * @return an integer represents exit code
 */
int main(int argc, char *argv[])
{
    if (wiringPiSetup() == -1) { //when initialize wiring failed,print message to screen
        printf("setup wiringPi failed !");
        return 1;
    }

    // init lcd, keypad
    {
        init_lcd();
        keypadInit();
    }

    // start ui here
    lcd_write(0, 0, "Welcome to Guess");
    lcd_write(0, 1, "Number: ");

    int result;
    struct options opts;
    options_init(&opts);
    result = parse_arguments(argc, argv, &opts);
    if (result != 0)
    {
        fprintf(stderr, "Usage: %s -o <proxy server address> [-p port_number]\n", argv[0]);         // NOLINT(cert-err33-c)
        exit(EXIT_FAILURE);                                                                         // NOLINT(concurrency-mt-unsafe)
    }

    struct sockaddr_in addr;
    struct sockaddr_in to_addr;
    struct sockaddr_in from_addr;
    struct timeval tv;
    tv.tv_sec = TIMEOUT_IN_SECOND;
    tv.tv_usec = 0;

    // open a socket
    opts.sock_fd = socket(AF_INET, SOCK_DGRAM, 0);       // NOLINT(android-cloexec-socket)
    if (opts.sock_fd == -1)
    {
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] open a socket", EXIT_FAILURE);
    }
    // set timeout
    setsockopt(opts.sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

    // init addr
    result = init_sockaddr(&addr);
    if (result != 0)
    {
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] initiate sockaddr_in", EXIT_FAILURE);
    }

    // bind
    result = bind(opts.sock_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if (result != 0)
    {
        close(opts.sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] bind", EXIT_FAILURE);
    }

    // init proxy_server addr
    result = init_proxy_sockaddr(&to_addr, &opts);
    if (result != 0)
    {
        close(opts.sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] initiate proxy server's sockaddr_in", EXIT_FAILURE);
    }

    result = do_client(&opts, &to_addr, &from_addr);
    if (result != -0)
    {
        close(opts.sock_fd);
        fatal_message(__FILE__, __func__, __LINE__, "[FAIL] sendto", EXIT_FAILURE);
    }

    close(opts.sock_fd);
    return EXIT_SUCCESS;
}


int main(int argc, char* argv[]) {


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
	int sockfd;
	struct sockaddr_in servaddr;
	socklen_t addlen = sizeof(servaddr);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket creation failed");
		return EXIT_FAILURE;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(PORT);

	char buffer[BUFFER] = { 0 };
	strcpy(buffer, user_num);
	buffer[4] = '\n';
	buffer[5] = '\0';
	printf("%s\n", buffer);
	if (connect(sockfd, (const struct sockaddr*) &servaddr, addlen) != 0) {
		perror("connection with the server failed\n");
	} else {
		write(sockfd, buffer, BUFFER);
		lcd_write(7, 1, "... OK");
	}

	close(sockfd);
    return 0;
}

