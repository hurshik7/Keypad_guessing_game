#ifndef KEYPAD_H
#define KEYPAD_H

#include <wiringPi.h>
#include <stdbool.h>
#include <stdio.h>

#define ROWS  4
#define COLS  4
#define BUTTON_NUM (ROWS * COLS)

void keypadInit(void);
void keyRead(unsigned char* result);
bool keyCompare(const unsigned char* a, const unsigned char* b);
void keyCopy(unsigned char* a, const unsigned char* b);
void keyPrint(const unsigned char* a);
void keyPrintLCD(unsigned char* a);
void keyClear(unsigned char* a);
int keyIndexOf(char value);
char getKey(unsigned char* a);

#endif /* KEYPAD_H */
