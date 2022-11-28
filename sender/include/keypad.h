#ifndef KEYPAD_H
#define KEYPAD_H

#include <wiringPi.h>
#include <stdbool.h>
#include <stdio.h>

#define ROWS  4
#define COLS  4
#define BUTTON_NUM (ROWS * COLS)

/**
 * Initialize the keypad.
 */
void keypadInit(void);

/**
 * Read a key from keypad.
 * @param result a pointer to char will contain the result
 */
void keyRead(unsigned char* result);

/**
 * Compare keys.
 * @param a an unsigned char*
 * @param b an unsigned char*
 * @return if two keys are same, return true, else false
 */
bool keyCompare(const unsigned char* a, const unsigned char* b);

/**
 * Copy a key. (from b, to a)
 * @param a an unsigned char*
 * @param b an unsigned char*
 */
void keyCopy(unsigned char* a, const unsigned char* b);

/**
 * Print a key.
 * @param a a pointer to unsigned char
 */
void keyPrint(const unsigned char* a);

/**
 * Print a key to LCD.
 * @param a an unsigned char*
 */
void keyPrintLCD(unsigned char* a);

/**
 * Clear a key.
 * @param a an unsigned char*
 */
void keyClear(unsigned char* a);

/**
 * Get an index of a key.
 * @param value a character
 * @return an integer
 */
int keyIndexOf(char value);

/**
 * Get a key.
 * @param a an unsigned char*
 * @return a character
 */
char getKey(unsigned char* a);

#endif /* KEYPAD_H */
