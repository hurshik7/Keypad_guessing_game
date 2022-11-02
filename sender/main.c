#include "distance.h"

#define DELAY_TIME (200)

int main() {
    float dis;
    if(wiringPiSetup() == -1){ //when initialize wiring failed,print message to screen
        printf("setup wiringPi failed !");
        return 1;
    }

    ultraInit();
    
    while(1){
        dis = disMeasure();

		// send data here
        printf("%0.2f cm\n\n",dis);
        delay(DELAY_TIME);
    }

    return 0;
}

