//
//  buttons.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-11-02.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#ifndef buttons_h
#define buttons_h

#include "global.h"

// MARK: Constants
#define BUTTON_DEBOUNCE_TIME    25

#define BUTTON_BUMPS_DIRTY      0
#define BUTTON_KEYS_DIRTY       1
#define BUTTON_PLAYBACKS_DIRTY  2


// MARK: Variable Declerations
extern volatile uint8_t buttons_bump_dirty;
extern volatile uint32_t buttons_keypad_dirty;

extern volatile uint8_t buttons_sections_dirty;

// MARK: Functions
/**
 * The code to be run in every main loop, debounces button inputs
 */
extern void button_service (void);


#endif /* buttons_h */
