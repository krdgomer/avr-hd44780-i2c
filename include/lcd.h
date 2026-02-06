#ifndef LCD_H
#define LCD_H
#include <avr/io.h>

// LCD configuration
#define LCD_ADDR 0x27  // I2C address (may be 0x3F on some modules)
#define LCD_COLS 16    // Number of columns (change to 20 for 20x4)
#define LCD_ROWS 2     // Number of rows (change to 4 for 20x4)

// PCF8574 pins
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RW        0x02
#define LCD_RS        0x01

// Function prototypes
void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char *str);
void lcd_print_char(char c);
void lcd_command(uint8_t cmd);
void lcd_backlight(uint8_t on);
void lcd_sliding_text(uint8_t row, const char *str);
void lcd_print_number(int16_t num);
void lcd_create_char(uint8_t location, uint8_t charmap[]);

#endif