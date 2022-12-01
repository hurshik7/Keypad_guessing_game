
#include "led.h"

void ledInit(void) {
    softPwmCreate(LedPinRed,  0, INIT_VAL);
    softPwmCreate(LedPinGreen,0, INIT_VAL);
    softPwmCreate(LedPinBlue, 0, INIT_VAL);
}

void ledColorSet(uchar r_val, uchar g_val, uchar b_val) {
    softPwmWrite(LedPinRed,   r_val);
    softPwmWrite(LedPinGreen, g_val);
    softPwmWrite(LedPinBlue,  b_val);
}

