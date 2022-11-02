/*********************************************************************
* Filename    : 1.1.2_rgbLed.c
* Description : Make a RGB LED emits various color.
* Author      : Robot
* E-mail      : support@sunfounder.com
* website     : www.sunfounder.com
* Update      : Daisy    2019/08/02
**********************************************************************/
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 5050
#define BUF_LEN 1024

#define uchar unsigned char
#define LedPinRed    0
#define LedPinGreen  1
#define LedPinBlue   2

void ledInit(void){
    softPwmCreate(LedPinRed,  0, 100);
    softPwmCreate(LedPinGreen,0, 100);
    softPwmCreate(LedPinBlue, 0, 100);
}

void ledColorSet(uchar r_val, uchar g_val, uchar b_val){
    softPwmWrite(LedPinRed,   r_val);
    softPwmWrite(LedPinGreen, g_val);
    softPwmWrite(LedPinBlue,  b_val);
}

int main(int argc, char *argv[]){

    if(wiringPiSetup() == -1){ //when initialize wiring failed, printf messageto screen
        printf("setup wiringPi failed !");
        return 1;
    }

	struct sockaddr_in addr;
    struct sockaddr from_addr;
    socklen_t from_addr_len;
    int result;
    int option;
    int fd;
    ssize_t nread;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if(fd == -1)
    {
        perror("scoket");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(addr.sin_addr.s_addr ==  (in_addr_t)-1)
    {
        perror("inet_addr");
        exit(EXIT_FAILURE);
    }

    option = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    result = bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    if(result == -1)
    {
        close(fd);
        perror("bind");
        exit(EXIT_FAILURE);
    }

    ledInit();
	float data;
    while(1){
		nread = recvfrom(fd, &data, sizeof(data), 0, &from_addr, &from_addr_len);
		if (nread == -1) {
			close(fd);
			perror("recvfrom");
        	exit(EXIT_FAILURE);
		} else {
			data = ntohs(data);
			printf("%f\n", data);
			if (data < 5) {
        		ledColorSet(0xff,0x00,0x00);   //red     
			} else {
        		ledColorSet(0x00,0xff,0x00);   //green
			}
		}
		
        //printf("Red\n");
        //ledColorSet(0xff,0x00,0x00);   //red     
        //delay(500);
        //printf("Green\n");
        //ledColorSet(0x00,0xff,0x00);   //green
    }
	close(fd);
    return 0;
}
