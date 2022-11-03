#include "distance.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DELAY_TIME (200)
#define PORT (5050)
#define BUF_LEN (1024)

void setup_wiringPi()
{
	if(wiringPiSetup() == -1) { //when initialize wiring failed,print message to screen
        printf("setup wiringPi failed !");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) 
{
	
   	float dis;
	setup_wiringPi();
    ultraInit();

	struct sockaddr_in addr;
    struct sockaddr_in to_addr;
    int result;
    int fd;
    char data[BUF_LEN];
    ssize_t nwrote;

    fd = socket(AF_INET, SOCK_DGRAM, 0); // NOLINT(android-cloexec-socket)

    if(fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE); // NOLINT(concurrency-mt-unsafe)
    }

    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(to_addr.sin_addr.s_addr == (in_addr_t) -1) // NOLINT(clang-analyzer-core.UndefinedBinaryOperatorResult)
    {
        perror("inet_addr");
        exit(EXIT_FAILURE); // NOLINT(concurrency-mt-unsafe)
    }

    result = bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    if(result == -1)
    {
        close(fd);
        perror("bind");
        exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
    }

    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(PORT);
    to_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(to_addr.sin_addr.s_addr ==  (in_addr_t)-1)
    {
        perror("inet_addr");
        exit(EXIT_FAILURE); // NOLINT(concurrency-mt-unsafe)
    }
    
    while(1) {
        dis = disMeasure();
		float dis_network_order = htons(dis);	
		// send data here
        printf("%0.2f cm\n",dis);
		nwrote = sendto(fd, &dis_network_order, sizeof(float), 0, (struct sockaddr *)&to_addr, sizeof(to_addr));

    	if(nwrote == -1) {
        	close(fd);
        	perror("recvfrom");
        	exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
    	} else {
			printf("success: send data!\n\n");
		}

        delay(DELAY_TIME);
    }

	close(fd);
    return 0;
}
