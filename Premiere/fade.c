//
//  fade.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-11-16.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "fade.h"

#include "global.h"
#include "dmx.h"

// MARK: Type definitions
struct Fade {
    uint8_t frame_step, flags;
    uint16_t channel, frame_period, last_frame;
};

// MARK: Constants
#define FADE_FLAGS_RUNNING      0
#define FADE_FLAGS_DONE         1

// MARK: Variables
static struct Fade fades[FADE_MAX_FADES];
static uint16_t fades_loaded;                               // Tracks which fades have been load to avoid segfaults


// MARK: Function definitions
void fade_load(uint8_t index, uint16_t channel, uint8_t target, uint16_t time) {
    struct Fade fade;
    
    uint8_t initial = dmx_get_dimmer(channel), steps;
    
    if (initial > target) {
        fade.frame_step = -1;
        steps = initial - target;
    } else {
        fade.frame_step = 1;
        steps = target - initial;
    }
    
    fade.frame_period = (steps * (TIMER_FREQUENCY/FADE_TIME_UNIT)) / time;
    fade.last_frame = 0;
    fade.channel = channel;
    fade.flags = 0;
//    fade.done = false;
    
    if (index < FADE_MAX_FADES) {                          // Make sure that index exists
//        fades[index] = fades;
        fades_loaded |= (1<<index);                        // Mark fade as loaded
    }
}

void fade_start(uint8_t index) {
    if ((index < FADE_MAX_FADES) && (fades_loaded & (1<<index))) {// Make sure that index exists and that an fade has been loaded to the index
        fades[index].flags |= (1<<FADE_FLAGS_RUNNING);
    }
}


void fade_stop (uint8_t index) {
    if ((index < FADE_MAX_FADES) && (fades_loaded & (1<<index))) {// Make sure that index exists and that an fade has been loaded to the index
        fades[index].flags &= ~(1<<FADE_FLAGS_RUNNING);
    }
}

uint8_t fade_next_open () {
    
}

void fade_service(void) {
    for (int i = 0; i < FADE_MAX_FADES; i++) {
        if ((fades_loaded & (1<<i)) && (fades[i].flags & (1<<FADE_FLAGS_RUNNING))) { // verify that a fade has been loaded to the index
            
            if ((millis - fades[i].last_frame) >= fades[i].frame_period) {
                
                // Codes goes here...
                
                fades[i].last_frame = millis;
            }
        }
    }
}
