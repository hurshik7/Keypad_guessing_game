#ifndef KEYPAD_H
#define KEYPAD_H

#include <wiringPi.h>
#include <stdbool.h>
#include <stdio.h>

#define ROWS  4
#define COLS  4
#define BUTTON_NUM (ROWS * COLS)

void keypadInit();
void keyRead(unsigned char* result);
bool keyCompare(unsigned char* a, unsigned char* b);
void keyCopy(unsigned char* a, unsigned char* b);
void keyPrint(unsigned char* a);
void keyPrintLCD(unsigned char* a);
void keyClear(unsigned char* a);
int keyIndexOf(const char value);
char getKey(unsigned char* a);

#endif /* KEYPAD_H */
