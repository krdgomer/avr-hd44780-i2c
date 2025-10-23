#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "lcd.h"
#include "i2c.h"

static uint8_t lcd_backlight_state = LCD_BACKLIGHT;

// Row addresses for different LCD configurations
static const uint8_t row_offsets_16x2[] = {0x00, 0x40};
static const uint8_t row_offsets_20x4[] = {0x00, 0x40, 0x14, 0x54};

static void lcd_pulse(uint8_t data) {
    i2c_write(data | LCD_ENABLE);
    _delay_us(1);
    i2c_write(data & ~LCD_ENABLE);
    _delay_us(50);
}

static void lcd_write_nibble(uint8_t nibble, uint8_t control) {
    uint8_t data = (nibble & 0xF0) | control | lcd_backlight_state;
    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(data);
    lcd_pulse(data);
    i2c_stop();
}

static void lcd_write_byte(uint8_t value, uint8_t control) {
    lcd_write_nibble(value & 0xF0, control);
    lcd_write_nibble((value << 4) & 0xF0, control);
}

void lcd_command(uint8_t cmd) {
    lcd_write_byte(cmd, 0);
    if (cmd == 0x01 || cmd == 0x02) _delay_ms(2);
}

void lcd_print_char(char c) {
    lcd_write_byte(c, LCD_RS);
}

void lcd_init(void) {
    _delay_ms(50);
    
    // Force into 4-bit mode
    lcd_write_nibble(0x30, 0);
    _delay_ms(5);
    lcd_write_nibble(0x30, 0);
    _delay_us(150);
    lcd_write_nibble(0x30, 0);
    _delay_us(150);
    lcd_write_nibble(0x20, 0);
    _delay_us(150);
    
    lcd_command(0x28); // 4-bit, 2 line, 5x8 font
    lcd_command(0x08); // display off
    lcd_command(0x01); // clear
    _delay_ms(2);
    lcd_command(0x06); // entry mode: increment, no shift
    lcd_command(0x0C); // display on, cursor off
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    if (row >= LCD_ROWS) row = LCD_ROWS - 1;
    if (col >= LCD_COLS) col = LCD_COLS - 1;
    
    uint8_t offset;
    #if LCD_ROWS == 4
        offset = row_offsets_20x4[row];
    #else
        offset = row_offsets_16x2[row];
    #endif
    
    lcd_command(0x80 | (offset + col));
}

void lcd_print(const char *str) {
    while (*str) lcd_print_char(*str++);
}

void lcd_clear(void) {
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_backlight(uint8_t on) {
    lcd_backlight_state = on ? LCD_BACKLIGHT : 0;
    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(lcd_backlight_state);
    i2c_stop();
}

void lcd_print_number(int16_t num) {
    char buffer[7];
    snprintf(buffer, sizeof(buffer), "%d", num);
    lcd_print(buffer);
}

void lcd_create_char(uint8_t location, uint8_t charmap[]) {
    location &= 0x7; // Only 8 locations (0-7)
    lcd_command(0x40 | (location << 3));
    for (uint8_t i = 0; i < 8; i++) {
        lcd_print_char(charmap[i]);
    }
}

void lcd_sliding_text(uint8_t row, const char *str) {
    const uint16_t slideSpeed = 300;
    
    char padded_str[LCD_COLS + 64];
    // Pad with spaces equal to LCD width
    char padding[LCD_COLS + 1];
    memset(padding, ' ', LCD_COLS);
    padding[LCD_COLS] = '\0';
    
    snprintf(padded_str, sizeof(padded_str), "%s%s%s", padding, str, padding);
    uint8_t padded_len = strlen(padded_str);
    
    char display_buf[LCD_COLS + 1];
    
    while(1) {
        for (uint8_t start_pos = 0; start_pos <= padded_len - LCD_COLS; start_pos++) {
            strncpy(display_buf, &padded_str[start_pos], LCD_COLS);
            display_buf[LCD_COLS] = '\0';
            
            // Only update the specific row, not the entire display
            lcd_set_cursor(0, row);
            lcd_print(display_buf);
            _delay_ms(slideSpeed);
        }
        _delay_ms(500);
    }
}