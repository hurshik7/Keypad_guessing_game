#ifndef GUESSING_SERVER_LED_H
#define GUESSING_SERVER_LED_H

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

#define uchar unsigned char
#define LedPinRed    26
#define LedPinGreen  27
#define LedPinBlue   21

void ledInit(void);

void ledColorSet(uchar r_val, uchar g_val, uchar b_val);

#endif //GUESSING_SERVER_LED_H
