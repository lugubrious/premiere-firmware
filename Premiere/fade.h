//
//  fade.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-11-16.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

// ! Not really impelmented !

#ifndef fade_h
#define fade_h

#include "global.h"

// MARK: Constants
#define FADE_MAX_FADES          10      // That is one awkwardly worded constant name...

#define FADE_TIME_UNIT          1000    // Fade times are in milliseconds
#define FADE_MAX_TIME           13107   // Maximum fade time

// MARK: Variable declerations


// MARK: Fuction declerations

/**
 * Gets a new fade ready to run
 * @param index Which index the fade should be palced in
 * @param channel Which channel to fade
 * @param taget The value the channel should be faded to
 * @param time How long it shoudl take the fade to complete
 */
extern void fade_load(uint8_t index, uint16_t channel, uint8_t target, uint16_t time);

/**
 * Starts an fade running
 * @param index Which fade should be started
 */
extern void fade_start(uint8_t index);

/**
 * Stops an fade from running
 * @param index Which fade should be stopped
 */
extern void fade_stop(uint8_t index);

/**
 * To be run in every main loop
 */
extern void fade_service(void);

#endif /* fade_h */
