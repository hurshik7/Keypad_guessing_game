#ifndef GUESSING_SERVER_LED_H
#define GUESSING_SERVER_LED_H

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

#define uchar unsigned char
#define LedPinRed    0
#define LedPinGreen  1
#define LedPinBlue   2
#define INIT_VAL (100)

/**
 * Initiate LED.
 */
void ledInit(void);

/**
 * Set a color of LED.
 * @param r_val an unsigned char represents red value
 * @param g_val an unsigned char represents green value
 * @param b_val an unsigned char represents blue value
 */
void ledColorSet(uchar r_val, uchar g_val, uchar b_val);

#endif //GUESSING_SERVER_LED_H
