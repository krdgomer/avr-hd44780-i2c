#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "i2c.h"


void lcd_pulse(uint8_t data) {
    i2c_write(data | LCD_ENABLE);
    _delay_us(1);
    i2c_write(data & ~LCD_ENABLE);
    _delay_us(50);
}

void lcd_write_nibble(uint8_t nibble, uint8_t control) {
    uint8_t data = (nibble & 0xF0) | control | LCD_BACKLIGHT;
    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(data);
    lcd_pulse(data);
    i2c_stop();
}

void lcd_write_byte(uint8_t value, uint8_t control) {
    lcd_write_nibble(value & 0xF0, control);
    lcd_write_nibble((value << 4) & 0xF0, control);
}


void lcd_command(uint8_t cmd) {
    lcd_write_byte(cmd, 0);
    if (cmd == 0x01 || cmd == 0x02) _delay_ms(2);
}

void lcd_data(uint8_t data) {
    lcd_write_byte(data, LCD_RS);
}


void lcd_init(void) {
    _delay_ms(50);  // Wait for LCD power

    // Force into 4-bit mode
    lcd_write_nibble(0x30, 0);
    _delay_ms(5);
    lcd_write_nibble(0x30, 0);
    _delay_us(150);
    lcd_write_nibble(0x30, 0);
    _delay_us(150);
    lcd_write_nibble(0x20, 0);  // 4-bit mode
    _delay_us(150);

    lcd_command(0x28); // 4-bit, 2 line, 5x8 font
    lcd_command(0x08); // display off
    lcd_command(0x01); // clear
    _delay_ms(2);
    lcd_command(0x06); // entry mode
    lcd_command(0x0C); // display on, cursor off
}


void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t addr = (row == 0) ? 0x80 : 0xC0;
    lcd_command(addr + col);
}

void lcd_print(const char *str) {
    while (*str) lcd_data(*str++);
}



