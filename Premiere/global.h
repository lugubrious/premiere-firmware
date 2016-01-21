//
//  global.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-01.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include <stdint.h>                   // Int types are needed everywehre
#include <stdbool.h>
#include <avr/pgmspace.h>

#ifndef global_h
#define global_h

// MARK: Definitions
#define DEBUG_MODE

#define TIMER_FREQUENCY     1000

// MARK: Global variables
extern volatile uint32_t millis; // Tracks the number of milliseconds elapsed since initilization

extern volatile uint8_t adc_values[8];

#endif /* global_h */


// Fuse Settings:   LOW:FF       HIGH:D7         EXTENDED:FD

// Which means:
//  External Crystal Oscilator, 8MHz+, Start up time 16CK + 65 ms
//  No clock out
//  Boot reset vector not enabled
//  Boot flash section size = 512 words, start address = $7E00
//  Preserve EEPROM memory through chip erase cycle
//  Watch dog timer not always on
//  SPI programming enabled
//  JTAG debug disabled
//  On-chip debug disabled
//  Brown-out detection level = 2v7

// Selected with: http://www.engbedded.com/fusecalc