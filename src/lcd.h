#ifndef LCD_H
#define LCD_H
#include <avr/io.h>

#define LCD_ADDR 0x27  // I2C address of PCF8574 (may be 0x3F on some modules)
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RW        0x02
#define LCD_RS        0x01

void lcd_pulse(uint8_t data);
void lcd_write_nibble(uint8_t nibble, uint8_t control);
void lcd_write_byte(uint8_t value, uint8_t control);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_init(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char *str);

#endif