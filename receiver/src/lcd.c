#include "lcd.h"

#define FIVE_MILLI_SECONDS (5)

static int fd;                              // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
static int BLEN = 1;                        // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
static const int LCDAddr = 0x27;             // NOLINT(cppcoreguidelines-avoid-non-const-global-variables, readability-magic-numbers, cppcoreguidelines-avoid-magic-numbers)

void write_word(int data){
    int temp = data;
    if (BLEN == 1) {
        temp |= 0x08;                       // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    } else {
        temp &= 0xF7;                       // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    }
    wiringPiI2CWrite(fd, temp);
}

void send_command(int comm){
    int buf;
    // Send bit7-4 firstly
    buf = comm & 0xF0;              // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    buf |= 0x04;                    //NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    write_word(buf);
    delay(2);
    buf &= 0xFB;                    // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    write_word(buf);

    // Send bit3-0 secondly
    buf = (comm & 0x0F) << 4;       // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    buf |= 0x04;                    // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    write_word(buf);
    delay(2);
    buf &= 0xFB;                    // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    write_word(buf);
}

void send_data(int data){
    int buf;
    // Send bit7-4 firstly
    buf = data & 0xF0;              // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    buf |= 0x05;                    // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    write_word(buf);
    delay(2);
    buf &= 0xFB;                    // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    write_word(buf);

    // Send bit3-0 secondly
    buf = (data & 0x0F) << 4;       // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    buf |= 0x05;                    // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    write_word(buf);
    delay(2);
    buf &= 0xFB;                    // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    write_word(buf);
}

void init_lcd(void) {
	fd = wiringPiI2CSetup(LCDAddr);
    send_command(0x33);     // Must initialize to 8-line mode at first //NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    delay(FIVE_MILLI_SECONDS);
    send_command(0x32);     // Then initialize to 4-line mode           // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    delay(FIVE_MILLI_SECONDS);
    send_command(0x28);     // 2 Lines & 5*7 dots                       // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    delay(FIVE_MILLI_SECONDS);
    send_command(0x0C);     // Enable display without cursor            // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    delay(FIVE_MILLI_SECONDS);
    send_command(0x01);     // Clear Screen                             // NOLINT(hicpp-signed-bitwise,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    wiringPiI2CWrite(fd, 0x08);                                         // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

void lcd_clear(void) {
    send_command(0x01);     //clear Screen
}

void lcd_write(int x, int y, const char data[]) {
    int addr;
    int i;
    int tmp;
    if (x < 0) {
        x = 0;
    }
    if (x > 15) {                               // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        x = 15;                                 // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    }
    if (y < 0) {
        y = 0;
    }
    if (y > 1) {
        y = 1;
    }

    // Move cursor
    addr = 0x80 + 0x40 * y + x;                 // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    send_command(addr);

    tmp = strlen(data);                         // NOLINT(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
    for (i = 0; i < tmp; i++) {
        send_data(data[i]);
    }
}

void ledInit(void) {
    softPwmCreate(LedPinRed,  0, 100);
    softPwmCreate(LedPinGreen,0, 100);
    softPwmCreate(LedPinBlue, 0, 100);
}

void ledColorSet(uchar r_val, uchar g_val, uchar b_val){
    softPwmWrite(LedPinRed,   r_val);
    softPwmWrite(LedPinGreen, g_val);
    softPwmWrite(LedPinBlue,  b_val);
}
