//
//  Shift.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-01.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "shift.h"

#include <avr/io.h>
#include <avr/cpufunc.h>                                        // Import avr cpu functions [_NOP()]
#include <util/delay.h>                     // Include avr utils delay, it makes busy waiting easy



// MARK: Variable Definitions
uint16_t shift_out_buffer;
uint32_t shift_in_buffer;

void shift_out (void) {
    SHIFT_OUT_SCLK_PORT &= ~(1<<SHIFT_OUT_SCLK_NUM);            // Make sure SCLK is low
    SHIFT_OUT_RCLK_PORT &= ~(1<<SHIFT_OUT_RCLK_NUM);            // Set RCLK low for data to be shifted in
    for (int i = 0; i < (SHIFT_OUT_NUM_REGISTERS * 8); i++) {   // Iterate through bits
        SHIFT_OUT_SCLK_PORT &= ~(1<<SHIFT_OUT_SCLK_NUM);
        if (shift_out_buffer & (1<<i)) {
            SHIFT_OUT_DATA_PORT |= (1<<SHIFT_OUT_DATA_NUM);     // If bit is a one, write data pin high
        } else {
            SHIFT_OUT_DATA_PORT &= ~(1<<SHIFT_OUT_DATA_NUM);    // If bit is a zero, write data pin low
        }
        SHIFT_OUT_SCLK_PORT |= (1<<SHIFT_OUT_SCLK_NUM);         // Write SCLK high to shift bit in
        SHIFT_OUT_SCLK_PORT &= ~(1<<SHIFT_OUT_SCLK_NUM);        // Write SCLK low to get ready for next bit
    }
    SHIFT_OUT_RCLK_PORT |= (1<<SHIFT_OUT_RCLK_NUM);             // Write RCLK high to push register out
}

uint8_t shift_in (void) {
    static uint32_t shift_in_prev_buffer;
    
    // Load
    SHIFT_IN_LOAD_PORT |= (1<<SHIFT_IN_LOAD_NUM);               // Make sure parallel load is high
    
    SHIFT_IN_CLK_PORT &= ~(1<<SHIFT_IN_CLK_NUM);                // Pulse CLK pin
    SHIFT_IN_CLK_PORT |= (1<<SHIFT_IN_CLK_NUM);
    
    SHIFT_IN_LOAD_PORT &= ~(1<<SHIFT_IN_LOAD_NUM);              // Pulse pload pin
    SHIFT_IN_LOAD_PORT |= (1<<SHIFT_IN_LOAD_NUM);
    
    // Get data
    for (int i = 0; i < (8 * SHIFT_IN_NUM_REGISTERS); i++) {    // Iterate through bits
        SHIFT_IN_CLK_PORT |= (1<<SHIFT_IN_CLK_NUM);             // Pull CLK high
        //♭♭ Every cycle is sacred, every cycle is great. If a cycle is wasted, Sam gets quite irate. ♭♭
        _delay_ms(.1);                                          // Wait a clock cycle so register can settle, a single cycle should be sufficient since the register can work at clock speeds of up to at least 25MHz. The _NOP() macro guarantees a NOP instruction that will not be optomized out.
        if ((SHIFT_IN_DATA_PIN & (1<<SHIFT_IN_DATA_NUM))) { // Update the relivant bit
            shift_in_buffer |= (1<<i);
        } else {
//            shift_out_buffer |= (1<<11);
            shift_in_buffer &= ~(1<<i);
        }
        SHIFT_IN_CLK_PORT &= ~(1<<SHIFT_IN_CLK_NUM);            // Pull CLK low
    }
    
    uint8_t ret_value = 0;
    if (shift_in_buffer != shift_in_prev_buffer) {
        ret_value = 1;
    }
    shift_in_prev_buffer = shift_in_buffer;
    return ret_value;
}