//
//  LCD.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-01.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "lcd.h"

#include "i2cmaster.h"                      // Include I2C library
#include <string.h>                         // Include avr string functions
#include <stdlib.h>                         // Include avr standard library [ itoa() ]
#include <util/delay.h>                     // Include avr utils delay, it makes busy waiting easy
#include "font.h"                           // Include the custom chracters

#include "shift.h"

// MARK: Variable Definitions
uint8_t lcd_backlight_mask, lcd_settings = LCD_DEFAULT_SETTINGS;

// MARK: Function Definitions

void lcd_init (void) {
    i2c_init();
    
    lcd_backlight_mask = (1<<LCD_BACKLIGHT_PIN); // Backlight is on by default
    
    
    // I'm not quite sure why the lcd has to be set to 4 bits three times, but it doesn't work an other way
    
    // TODO: Find out what is happenng here
    lcd_write_raw_byte(0x4);                    // Set to 4 bit (only using 4 bits)
    lcd_command(0x28);                          // Set to 4 bit, 2 lines, 5x7 font (with all 8 bits now)
    lcd_write_raw_byte(0x4);                    // Set to 4 bit again (with only 4 bits again?)
    
    lcd_clear();
//    lcd_command(0xc);                           // Set cursor mode
    
    lcd_write_settings();
    
    for (int i = 0; i < FONT_NUM_CHARS; i++) {  // Upload the custom font
        lcd_upload_custom_char(FONT[i], i+1);
    }
}

void lcd_write_settings (void) {
    uint8_t entry_mode = LCD_ENTRY_MODE_CMD,
            disp_on_off = LCD_ON_OFF_CMD,
            cursor_shift_mode = LCD_CURSOR_SHIFT_CMD;
    
    if (lcd_settings & (1<<LCD_SET_MOVE_DIR)) {
        entry_mode |= (1<<1);
    }
    if (lcd_settings & (1<<LCD_SET_DISPLAY_SHIFT)) {
        entry_mode |= (1<<0);
    }
    
    if (lcd_settings & (1<<LCD_SET_DISPLAY_ON)) {
        disp_on_off |= (1<<2);
    }
    if (lcd_settings & (1<<LCD_SET_CURSOR)) {
        disp_on_off |= (1<<1);
    }
    if (lcd_settings & (1<<LCD_SET_CURSOR_BLINK)) {
        disp_on_off |= (1<<0);
    }
    
    if (lcd_settings & (1<<LCD_SET_MOVE_OR_SHIFT)) {
        cursor_shift_mode |= (1<<1);
    }
    if (lcd_settings & (1<<LCD_SET_SHIFT_DIR)) {
        cursor_shift_mode |= (1<<0);
    }
    
    lcd_command(entry_mode);
    lcd_command(disp_on_off);
    lcd_command(cursor_shift_mode);
}

void lcd_upload_custom_char(const uint8_t character[], uint8_t index) {
    if (index == 8) { index = 0; }               // Since we can't use char 0 in strings we move swap it with char 8
    lcd_command((index<<3) | LCD_CGRAM_ADDR_CMD);// Set cgram address to first address of char to be writen
    for (int i = 0; i < LCD_CHAR_LINES; i++) {   // Write bytes of char
        lcd_data(character[i]);
    }
}

void lcd_clear (void) {
    lcd_command(LCD_DISPLAY_CLEAR_CMD);
}

void lcd_cursor_home (void) {
    lcd_command(LCD_CURSOR_HOME_CMD);
}

void lcd_set_backlight(void) {
    lcd_backlight_mask = (1<<LCD_BACKLIGHT_PIN);// Set mask so that backlight will be kept on
    lcd_write_raw_byte(lcd_backlight_mask);     // Turn backlight on
}

void lcd_unset_backlight(void) {
    lcd_backlight_mask = 0;                     // Set mask so that backlight will be kept off when data is sent
    lcd_write_raw_byte(lcd_backlight_mask);     // Turn backlight off
}

void lcd_command (uint8_t data) {
    lcd_write_nibble((data & 0xF0), LCD_COMMAND);// Sent first four bytes of command
    lcd_write_nibble((data << 4), LCD_COMMAND);  // Sent next four bytes of command
}

void lcd_data (uint8_t data) {
    lcd_write_nibble((data & 0xF0), LCD_DATA);  // Sent only the first four bytes of data
    lcd_write_nibble((data << 4), LCD_DATA);    // Sent only the next four bytes of data
}

void lcd_write_nibble(uint8_t value, uint8_t mode) {
    if (mode == LCD_DATA) {                     // If we are sending data the RS pin needs to be high
        mode = (1<<LCD_RS_PIN);
    }
    
    value |= mode | lcd_backlight_mask;         // Apply mode and backlight masks to value
    lcd_write_raw_byte(value);                  // Write the data
}


// TODO: Find a way to recreate this function without busy waiting
void lcd_write_raw_byte (uint8_t data) {
    // TODO: Check if I actually need to use start_wait, or if start then rep_start will work
//    i2c_start_wait((0x27<<1) + I2C_WRITE); // Start talking to the I2C device
    i2c_start((0x27<<1) + I2C_WRITE);
    // I have no idea why the address has to be bit shifted left, which is why I spent 3 (explitive deleted) days debugging the I2C output code before copying the bit shifting from some example code -_- But now it works, and I can sleep *sigh*
    
    
    i2c_write(data | (1<<LCD_EN_PIN));          // En HIGH
    _delay_ms(.7);                              // We need a delay since the LCD is slower than the MCU
    i2c_write(data & ~(1<<LCD_EN_PIN));         // En LOW
    _delay_ms(.7);                              // Eek! 11200 waisted clock cycles! (twice) ):

    i2c_stop();
}


void lcd_write_char(char c, int8_t location) {
    if (location != LCD_LOCATION_CURSOR)        // Set DDRAM address to location, if the location is 'cursor' the char will be writen to the current cursor location
        lcd_command(location | LCD_DDRAM_ADDR_CMD); // Update the cursor location
    lcd_data(c);                                // We can write chars directly since the display uses a subset of ASCII (mostly, '|' has been replaced with '¥', and everything from 176 ('~') on is non-standard)
}

void lcd_write_int(int d, uint8_t digits, int8_t location) {
    char string[digits+1];                      // Space for digits and a null terminator
    for (int i = 0; i < digits; i++) {
        string[i] = ' ';
    }
    itoa(d, string, 10);                        // Convert d to string (base ten)
    bool replacing = false;
    for (int i = 0; i < digits; i++) {
        if  (replacing) {
            string[i] = ' ';
        } else if (string[i] == 0) {
            replacing = true;
            i--;
        }
    }
    lcd_write_string(string, location);         // Write the string
}

void lcd_write_percentage(uint8_t byte, int8_t location) {
    uint8_t step = ((uint16_t)byte + 31) / 32;  // Convert to value beetween zero and eight (inclusive)
    lcd_write_char(FONT_PERCENTAGE_INDICATOR[step], location);
}


void lcd_write_string(const char* c, int8_t location) {
    uint8_t length = strlen(c);                 // Get length of string
    if (length) {                               // If the string has no chars return
        lcd_write_char(c[0], location);         // Write the first char to the specified location
        for (int i = 1; i < length; i++) {      // Write the following chars after the first char
            if (c[i] == '\n') {          // The new line char simply jumps to the begining of line two, which should work fine for all the uses I need
                lcd_command(LCD_LINE_TWO_START | LCD_DDRAM_ADDR_CMD);
            } else {
                lcd_write_char(c[i], LCD_LOCATION_CURSOR);
            }
        }
    }
}