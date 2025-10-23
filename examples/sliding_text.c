#include "lcd.h"
#include "i2c.h"
#include <util/delay.h>

int main(void) {
    i2c_init();
    lcd_init();
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Row 0: Static");
    lcd_sliding_text(1, "This text slides!");
    
    while (1) { }
}