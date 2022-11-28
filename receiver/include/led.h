//
// Created by Shik Hur on 2022-11-28.
//

#ifndef GUESSING_SERVER_LED_H
#define GUESSING_SERVER_LED_H

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

#define uchar unsigned char
#define LedPinRed    28
#define LedPinGreen  29
#define LedPinBlue   23

void ledInit(void);

void ledColorSet(uchar r_val, uchar g_val, uchar b_val);

#endif //GUESSING_SERVER_LED_H
