#include "lcd.h"
#include "i2c.h"

int main(void) {
    i2c_init();
    lcd_init();

    lcd_set_cursor(0, 0);
    lcd_print("Hello World!");

    lcd_set_cursor(0, 1);
    lcd_print("Line Two");

    while (1) { }
}