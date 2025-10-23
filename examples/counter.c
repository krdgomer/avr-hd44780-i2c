#include "lcd.h"
#include "i2c.h"
#include <util/delay.h>

int main(void) {
    i2c_init();
    lcd_init();
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Counter: ");
    for (int i = 0; i < 50; i++) {
        lcd_set_cursor(9, 0);
        lcd_print_number(i);
        _delay_ms(200);
    }
    
    while (1) { }
}