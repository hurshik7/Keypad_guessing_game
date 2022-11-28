#ifndef DISTANCE_H
#define DISTNACE_H

#include <wiringPi.h>
#include <stdio.h>
#include <sys/time.h>

#define Trig    4
#define Echo    5

void ultraInit(void);
float disMeasure(void);

#endif // DISTANCE_H

