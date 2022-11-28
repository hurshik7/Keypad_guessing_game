#include "lcd.h"

static int fd;                              // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
static int BLEN = 1;                        // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
static int LCDAddr = 0x27;                  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables,readability-magic-numbers)

void write_word(int data){
    int temp = data;
    if (BLEN == 1) {
        temp |= 0x08;
    } else {
        temp &= 0xF7;
    }
    wiringPiI2CWrite(fd, temp);
}

void send_command(int comm){
    int buf;
    // Send bit7-4 firstly
    buf = comm & 0xF0;
    buf |= 0x04;                    // RS = 0, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;                    // Make EN = 0
    write_word(buf);

    // Send bit3-0 secondly
    buf = (comm & 0x0F) << 4;
    buf |= 0x04;                    // RS = 0, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;                    // Make EN = 0
    write_word(buf);
}

void send_data(int data){
    int buf;
    // Send bit7-4 firstly
    buf = data & 0xF0;
    buf |= 0x05;                    // RS = 1, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;                    // Make EN = 0
    write_word(buf);

    // Send bit3-0 secondly
    buf = (data & 0x0F) << 4;
    buf |= 0x05;                    // RS = 1, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;                    // Make EN = 0
    write_word(buf);
}

void init_lcd() {
	fd = wiringPiI2CSetup(LCDAddr);
    send_command(0x33);     // Must initialize to 8-line mode at first
    delay(5);
    send_command(0x32);     // Then initialize to 4-line mode
    delay(5);
    send_command(0x28);     // 2 Lines & 5*7 dots
    delay(5);
    send_command(0x0C);     // Enable display without cursor
    delay(5);
    send_command(0x01);     // Clear Screen
    wiringPiI2CWrite(fd, 0x08);
}

void lcd_clear() {
    send_command(0x01);     //clear Screen
}

void lcd_write(int x, int y, const char data[]) {
    int addr, i;
    int tmp;
    if (x < 0) {
        x = 0;
    }
    if (x > 15) {
        x = 15;
    }
    if (y < 0) {
        y = 0;
    }
    if (y > 1) {
        y = 1;
    }

    // Move cursor
    addr = 0x80 + 0x40 * y + x;                 // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    send_command(addr);

    tmp = strlen(data);                         // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
    for (i = 0; i < tmp; i++) {
        send_data(data[i]);
    }
}
