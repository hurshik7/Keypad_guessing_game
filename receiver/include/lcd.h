#ifndef LCD_H
#define LCD_H

#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <softPwm.h>
#include <string.h>

#define uchar unsigned char
#define LedPinRed    0
#define LedPinGreen  1
#define LedPinBlue   2

void write_word(int data);
void send_command(int comm);
void send_data(int data);
void init_lcd(void);
void lcd_clear(void);
void lcd_write(int x, int y, const char data[]);
void ledInit(void);
void ledColorSet(uchar r_val, uchar g_val, uchar b_val);

#endif /* LCD_H */

