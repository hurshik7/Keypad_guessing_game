#include "distance.h"

void ultraInit(void)
{
    pinMode(Echo, INPUT);
    pinMode(Trig, OUTPUT);
}

float disMeasure(void)
{
    struct timeval tv1;
    struct timeval tv2;
    long time1, time2;
   float dis;

    digitalWrite(Trig, LOW);
    delayMicroseconds(2);

    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);      
    digitalWrite(Trig, LOW);
                                
    while(!(digitalRead(Echo) == 1));   
    gettimeofday(&tv1, NULL);           

    while(!(digitalRead(Echo) == 0));   
    gettimeofday(&tv2, NULL);           

    time1 = tv1.tv_sec * 1000000 + tv1.tv_usec;   
    time2  = tv2.tv_sec * 1000000 + tv2.tv_usec;

    dis = (float)(time2 - time1) / 1000000 * 34000 / 2;  

    return dis;
}

