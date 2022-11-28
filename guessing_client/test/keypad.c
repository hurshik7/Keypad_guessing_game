#include "keypad.h"

unsigned char KEYS[BUTTON_NUM] = {
	'1','2','3','A',
	'4','5','6','B',
	'7','8','9','C',
	'*','0','#','D'
};

static unsigned char rowPins[ROWS] = {1, 4, 5, 6};
static unsigned char colPins[COLS] = {12, 3, 2, 0};

void keypadInit(void) {
    for (int i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        pinMode(colPins[i], INPUT);
    }
}

void keyRead(unsigned char* result) {
    int index;
    int count = 0;
    keyClear(result);
    for (int i = 0; i < ROWS; i++) {
        digitalWrite(rowPins[i], HIGH);
        for (int j = 0; j < COLS; j++) {
            index = i * ROWS + j;
            if (digitalRead(colPins[j]) == 1){
                result[count] = KEYS[index];
                count += 1;
            }
        }
        delay(1);
        digitalWrite(rowPins[i], LOW);
    }
}

bool keyCompare(unsigned char* a, unsigned char* b) {
    for (int i = 0; i < BUTTON_NUM; i++){
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void keyCopy(unsigned char* a, unsigned char* b){
    for (int i = 0; i < BUTTON_NUM; i++){
        a[i] = b[i];
    }
}

void keyPrint(unsigned char* a) {
    if (a[0] != 0){
        printf("%c",a[0]);
    }
    for (int i = 1; i < BUTTON_NUM; i++) {
        if (a[i] != 0){
            printf(", %c",a[i]);
        }
    }
    printf("\n");
}

void keyClear(unsigned char* a) {
    for (int i = 0; i < BUTTON_NUM; i++) {
        a[i] = 0;
    }
}

int keyIndexOf(const char value){
    for (int i = 0; i < BUTTON_NUM; i++){
        if ((const char)KEYS[i] == value){
            return i;
        }
    }
    return -1;
}

char getKey(unsigned char* a) {
	for (int i = 0; i < BUTTON_NUM; i++) {
		if (a[i] != 0) {
			return a[i];
		}
	}
	return '\0';
}

