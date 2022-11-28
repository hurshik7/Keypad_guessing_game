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


int LCDAddr = 0x27;
int BLEN = 1;
int fd1;





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

void write_word(int data) {
    int temp = data;
    if(BLEN == 1){
        temp |= 0x08;
    
    } else {
        temp &= 0xF7;
    }
    wiringPiI2CWrite(fd, temp);
}



void send_command(int comm) {
    int buf;

    buf = comm & 0xF0;
    buf |= 0x04;
    write_word(buf);
    delay(2);
    buf &= 0xFB;
    write_word(buf);


    buf = (comm & 0x0F) << 4;
    buf |= 0x04;
    write_word(buf);
    delay(2);
    buf &= 0xFB;
    write_word(buf);

}

void send_data(int data){
    int buf;
    // Send bit7-4 firstly
    buf = data & 0xF0;
    buf |= 0x05;                    // RS = 1, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;                    // Make EN = 0
    write_word(buf);

    // Send bit3-0 secondly
    buf = (data & 0x0F) << 4;
    buf |= 0x05;                    // RS = 1, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;                    // Make EN = 0
    write_word(buf);
}

void init(){
    send_command(0x33);     // Must initialize to 8-line mode at first
    delay(5);
    send_command(0x32);     // Then initialize to 4-line mode
    delay(5);
    send_command(0x28);     // 2 Lines & 5*7 dots
    delay(5);
    send_command(0x0C);     // Enable display without cursor
    delay(5);
    send_command(0x01);     // Clear Screen
    wiringPiI2CWrite(fd, 0x08);
}

void clear(){
    send_command(0x01);     //clear Screen
}

void write(int x, int y, char data[]){
    int addr, i;
    int tmp;
    if (x < 0)  x = 0;
    if (x > 15) x = 15;
    if (y < 0)  y = 0;
    if (y > 1)  y = 1;

    // Move cursor
    addr = 0x80 + 0x40 * y + x;
    send_command(addr);

    tmp = strlen(data);
    for (i = 0; i < tmp; i++){
        send_data(data[i]);
    }
}


//returns a string of two strings combined.
char *concat(const char *a, const char *b){
    int lena = strlen(a);
    int lenb = strlen(b);
    char *con = malloc(lena+lenb+1);
    //copies and concat (including the string termination).
    memcpy(con,a,lena);
    memcpy(con+lena,b,lenb+1);
    return con;
}


// compares the received data to the targer value and returns a message to the receiver.
const char* getStatus(double val, double targer){
    if(val == num) {
        ledColorSet(0x00,0xff,0x00);
        return "Good!";
    } else {
        ledColorSet(0xff,0x00,0x00);
        return "Bad!";
    }
}


// compares the target and sends a response to the sender according to the data received.
const char* giveResponse(double val, double targer){
    if(val > targer){
        return "Go Lower!";
    } else if (val < targer){
        return "Go Higher!";
    } else {
        return "Congratulations";
    }
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
        char str[100];
		nread = recvfrom(fd, &data, sizeof(data), 0, &from_addr, &from_addr_len);
		if (nread == -1) {
			close(fd);
			perror("recvfrom");
        	exit(EXIT_FAILURE);
		} else {
			data = ntohs(data);
            sprintf(str, "%f", data);
			printf("%f\n", data);
            char str1[10] = "Status: ";
            char str2 [10] = "Number: ";

            //create string for LCD display
            char *con = concat(str1, getStatus(data, 5));
            char *con1 = concat(str2, str);

            fd1 = wiringPiI2CSetup(LCDAddr);
            init();

            // write to LCD display
            writes(0,0,con);
            writes(1,1,con1);

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
