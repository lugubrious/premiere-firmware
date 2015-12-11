//
//  Shift.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-01.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "pindefinitions.h"
#include "global.h"

#ifndef Shift_h
#define Shift_h

// MARK: Constants
#define SHIFT_IN_NUM_REGISTERS  1
#define SHIFT_OUT_NUM_REGISTERS 2

// MARK: Variable Declerations
extern uint16_t shift_out_buffer;       // The data to be shifted out
extern uint32_t shift_in_buffer;        // The most recent data from the shift registers

// MARK: Functions
/**
 * Shifts the buffer out to the shift registers
 */
extern void shift_out(void);

/**
 * Shifts the data from the shift registers into the buffer
 * @return 0 if no values changed since last shift in, 1 if values have changed
 */
extern uint8_t shift_in(void);

#endif /* Shift_h */


/*
Shift in Bit Order:
    Register:  1       2       3
    Pin:       abcdefghabcdefghabcdefgh
    Bit:       222211111111110000000000
               321098765432109876543210
*/

/*
 Shift out Bit Order:
    Register:  1       2
    Pin:       abcdefghabcdefgh
    Bit:       1111110000000000
               5432109876543210
 */