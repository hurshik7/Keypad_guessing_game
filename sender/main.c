#include "lcd.h"

void main() {
    init_lcd();
    lcd_write(0, 0, "Hello World");
    lcd_write(1, 1, "From COMP3980");
}

