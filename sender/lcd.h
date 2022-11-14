#ifndef LCD_H
#define LCD_H

#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>

void write_word(int data);
void send_command(int comm);
void send_data(int data);
void init_lcd();
void clear();
void lcd_write(int x, int y, char data[]);

#endif /* LCD_H */

