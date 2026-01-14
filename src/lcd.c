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

/**
 * Sends a pulse to the LCD to latch the data.
 *
 * @param data The data to be latched.
 */
static void lcd_pulse(uint8_t data) {
    i2c_write(data | LCD_ENABLE);
    _delay_us(1);
    i2c_write(data & ~LCD_ENABLE);
    _delay_us(50);
}

/**
 * Writes a 4-bit nibble to the LCD.
 *
 * @param nibble The 4-bit data to write.
 * @param control Control bits (e.g., RS for data/command).
 */
static void lcd_write_nibble(uint8_t nibble, uint8_t control) {
    uint8_t data = (nibble & 0xF0) | control | lcd_backlight_state;
    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(data);
    lcd_pulse(data);
    i2c_stop();
}

/**
 * Writes a full byte to the LCD.
 *
 * @param value The byte to write.
 * @param control Control bits (e.g., RS for data/command).
 */
static void lcd_write_byte(uint8_t value, uint8_t control) {
    lcd_write_nibble(value & 0xF0, control);
    lcd_write_nibble((value << 4) & 0xF0, control);
}

/**
 * Sends a command to the LCD.
 *
 * @param cmd The command byte to send.
 */
void lcd_command(uint8_t cmd) {
    lcd_write_byte(cmd, 0);
    if (cmd == 0x01 || cmd == 0x02) _delay_ms(2);
}

/**
 * Prints a single character to the LCD.
 *
 * @param c The character to print.
 */
void lcd_print_char(char c) {
    lcd_write_byte(c, LCD_RS);
}

/**
 * Initializes the LCD in 4-bit mode.
 */
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

/**
 * Sets the cursor to a specific row and column.
 *
 * @param col The column to set the cursor to.
 * @param row The row to set the cursor to.
 */
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

/**
 * Prints a string to the LCD.
 *
 * @param str The null-terminated string to print.
 */
void lcd_print(const char *str) {
    while (*str) lcd_print_char(*str++);
}

/**
 * Clears the LCD display.
 */
void lcd_clear(void) {
    lcd_command(0x01);
    _delay_ms(2);
}

/**
 * Controls the LCD backlight.
 *
 * @param on Set to 1 to turn on the backlight, 0 to turn it off.
 */
void lcd_backlight(uint8_t on) {
    lcd_backlight_state = on ? LCD_BACKLIGHT : 0;
    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(lcd_backlight_state);
    i2c_stop();
}

/**
 * Prints a number to the LCD.
 *
 * @param num The number to print.
 */
void lcd_print_number(int16_t num) {
    char buffer[7];
    snprintf(buffer, sizeof(buffer), "%d", num);
    lcd_print(buffer);
}

/**
 * Creates a custom character in the LCD's CGRAM.
 *
 * @param location The CGRAM location (0-7) to store the character.
 * @param charmap An array of 8 bytes defining the character.
 */
void lcd_create_char(uint8_t location, uint8_t charmap[]) {
    location &= 0x7; // Only 8 locations (0-7)
    lcd_command(0x40 | (location << 3));
    for (uint8_t i = 0; i < 8; i++) {
        lcd_print_char(charmap[i]);
    }
}

/**
 * Displays a sliding text on the LCD.
 *
 * @param row The row where the sliding text will be displayed.
 * @param str The text to slide.
 */
void lcd_sliding_text(uint8_t row, const char *str) {
    const uint16_t slideSpeed = 1000;
    
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