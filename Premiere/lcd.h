//
//  LCD.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-01.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//
//  HD44780 display driver commands refrenced from http://www.ekenrooi.net/lcd/lcd.shtml
//

// TODO: Investigate feasibility/benifits of reimplementing I2C I/O as interupt driven.

//  LCD backpack is a "PCF8574 Remote 8-Bit I/O Expander for I2C Bus" from TI

#include "global.h"

#ifndef LCD_h
#define LCD_h

// MARK: Pin Definitions
#define I2C_ADDR 0x27

#define LCD_RS_PIN 0
#define LCD_RW_PIN 1
#define LCD_EN_PIN 2
#define LCD_BACKLIGHT_PIN 3

#define LCD_D4_PIN 4
#define LCD_D5_PIN 5
#define LCD_D6_PIN 6
#define LCD_D7_PIN 7

// MARK: Other macro definitions
#define LCD_COMMAND             0
#define LCD_DATA                1
#define LCD_LOCATION_CURSOR    -1

#define LCD_LINE_ONE_START      0
#define LCD_LINE_TWO_START      0x40
#define LCD_LINE_LENGTH         16
#define LCD_CHAR_LINES          8                           // Number of lines in a font character

#define LCD_DISPLAY_CLEAR_CMD   0x01
#define LCD_CURSOR_HOME_CMD     0x02
#define LCD_ENTRY_MODE_CMD      0x04
#define LCD_ON_OFF_CMD          0x08
#define LCD_CURSOR_SHIFT_CMD    0x10
#define LCD_FUNCTION_SET_CMD    0x20
#define LCD_CGRAM_ADDR_CMD      0x40
#define LCD_DDRAM_ADDR_CMD      0x80


#define LCD_SET_MOVE_DIR        0
#define LCD_SET_DISPLAY_SHIFT   1
#define LCD_SET_DISPLAY_ON      2
#define LCD_SET_CURSOR          3
#define LCD_SET_CURSOR_BLINK    4
#define LCD_SET_MOVE_OR_SHIFT   5
#define LCD_SET_SHIFT_DIR       6

#define LCD_DEFAULT_SETTINGS    0b00000101

// MARK: Variable Declarations
extern uint8_t lcd_backlight_mask, lcd_settings;

// MARK: Functions

// I2C functions
/**
 * Writes a byte to the I2C bus
 * @param data The data to be writen
 */
extern void lcd_write_raw_byte (uint8_t data);

// LCD functions
/**
 * Sets up the bus and writes intials settings to the LCD
 * @remark Must be called first
 */
extern void lcd_init (void);

/**
 * Writes the settings from the settings buffer to the proper registers in the LCD
 */
extern void lcd_write_settings (void);

/**
 * Sets up the bus and writes intials settings to the LCD
 * @remark I made indexes 0 and 8 point to the same place, which makes it possible to include custom char 0 in null terminated strings
 * @param character An array of bytes which represents the image of the char
 * @param index Where to locate the char
 */
extern void lcd_upload_custom_char(const uint8_t character[], uint8_t index);

/**
 * Changes the backlight mask so the the backlight will be kept on
 */
extern void lcd_set_backlight(void);

/**
 * Changes the backlight mask so the the backlight will be kept off
 */
extern void lcd_unset_backlight(void);

/**
 * Clears all charaters from the display
 */
extern void lcd_clear(void);

/**
 * Sends the cursor to loaction 0
 */
extern void lcd_cursor_home (void);

/**
 * Writes a command to the display
 * @param command The command to be written
 */
extern void lcd_command (uint8_t command);

/**
 * Writes some data to the display
 * @param data The data to be written
 */
extern void lcd_data (uint8_t data);

/**
 * Writes a nibble to the display (we only have access to 4 of the displays data pins)
 * @param value The data to be written
 * @param mode Command or Data, tells whether the command pin should be high during data write
 */
extern void lcd_write_nibble(uint8_t value, uint8_t mode);

/**
 * Writes a char to the display
 * @param c The char
 * @param location The location on the display where the char should be displayed
 */
extern void lcd_write_char(char c, int8_t location);

/**
 * Converts an int to a string and writes it to the display
 * @param d The int
 * @param digits How many digits to make space for in the string
 * @param location The location on the display where the string should be displayed
 */
extern void lcd_write_int(int d, uint8_t digits, int8_t location);

/**
 * Displays a percentage inidicator in a single char
 * @param byte The value to display
 * @param location The location on the display where the indicator should be displayed
 */
extern void lcd_write_percentage(uint8_t byte, int8_t location);

/**
 * Displays a string
 * @param c The string
 * @param location The location on the display where the string should be displayed
 */
extern void lcd_write_string(const char* c, int8_t location);


#endif /* LCD_h */
