//
//  DMX.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-01.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "global.h"

#ifndef DMX_h
#define DMX_h

// MARK: Constants
#define DMX_MAX_FRAME_FREQUENCY     34              // Number of milliseconds per packet
#define DMX_UNIVERSE_LENGTH         512             // The # of slots in a universe (defined by standard)
#define DMX_START_BYTE              0               // The start byte, 0 is a standard packet
#define DMX_NUM_SLOTS               512             // The # of DMX channels we are transmitting

// MARK: Variable Declarations
extern volatile uint8_t dmx_universe_one[512];      // The DMX output values for universe one
extern volatile uint8_t dmx_universe_two[512];      // The DMX output values for universe two

// MARK: Functions
/**
 * Assigns value to an address
 * @param dimmer The address, address >= 512 are in the second universe
 * @param value The value
 */
extern void dmx_set_dimmer(uint16_t dimmer, uint8_t value);

/**
 * Gets the value for an address
 * @param dimmer The address, address >= 512 are in the second universe
 * @return The value
 */
extern uint8_t dmx_get_dimmer(uint16_t dimmer);

/**
 * The code to be run in every main loop, basicly just starts sending a new packet at a regular interval
 */
extern void dmx_service (void);

/**
 * Initilize universe one
 */
extern void dmx_init_universe_one(void);

/**
 * Initilize universe two
 */
extern void dmx_init_universe_two(void);

/**
 * Start sending packets on universe one
 */
extern void dmx_one_start_frame(void);

/**
 * Start sending packets on universe two
 */
extern void dmx_two_start_frame(void);

/**
 * Stop sending packets on universe one
 */
extern void dmx_stop_universe_one(void);

/**
 * Stop sending packets on universe two
 */
extern void dmx_stop_universe_two(void);


#endif /* DMX_h */
