#include "lcd.h"
#include "i2c.h"
#include <util/delay.h>

int main(void) {
    i2c_init();
    lcd_init();
    
    uint8_t heart[8] = {
        0b00000,
        0b01010,
        0b11111,
        0b11111,
        0b01110,
        0b00100,
        0b00000,
        0b00000
    };
    lcd_create_char(0, heart);
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("I ");
    lcd_print_char(0); 
    lcd_print(" LCD");
    
    while (1) { }
}