#ifndef LCD_H
#define LCD_H

#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>

/**
 * Write a word into lcd.
 * @param data an integer
 */
void write_word(int data);

/**
 * Send a command.
 * @param comm an integer
 */
void send_command(int comm);

/**
 * Send a data
 * @param data an integer
 */
void send_data(int data);

/**
 * Initialize the lcd.
 */
void init_lcd(void);

/**
 * Clear the lcd.
 */
void lcd_clear(void);

/**
 * Prints a data at the specific location of lcd.
 * @param x an integer
 * @param y an integer
 * @param data char array represents data
 */
void lcd_write(int x, int y, const char data[]);

#endif /* LCD_H */

