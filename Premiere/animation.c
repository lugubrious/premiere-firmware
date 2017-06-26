//
//  animation.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-27.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "animation.h"

#include "lcd.h"

// MARK: Constant Definitions
const char ANIMATION_LOADING_CHARS[4] = {124, 47, 176, 1};
const double ANIMATION_LOADING_FRAMERATE = 5.0;
const uint8_t ANIMATION_LOADING_LENGTH = 4;

// MARK: Global Variable Definitions
static struct Animation animations[ANIMATION_NUM_ANIMATIONS];
static uint8_t animations_loaded;

// MARK: Function Definitions
struct Animation animation_create_new(uint8_t length, double frame_rate, int8_t location, const char* frames) {
    
    struct Animation ani;
    ani.length = length;
    ani.frame_delay = (uint8_t)(1000/frame_rate);               // T = 1/f
    ani.current_frame = 0;                                      // Start with first frame
    ani.location = location;
    ani.frames = frames;
    ani.next_frame_time = 0;                                    // Start with the first frame immediately
    ani.running = false;
    return ani;
}

void animation_load (struct Animation ani, uint8_t index) {
    if (index < ANIMATION_NUM_ANIMATIONS) {                     // Make sure that index exists
        animations[index] = ani;
        animations_loaded |= (1<<index);                        // Mark animation as loaded
    }
}

void animation_start (uint8_t index) {
    if ((index < ANIMATION_NUM_ANIMATIONS) && (animations_loaded & (1<<index))) {// Make sure that index exists and that an animation has been loaded to the index
        animations[index].running = true;
    }
}

void animation_stop (uint8_t index) {
    if ((index < ANIMATION_NUM_ANIMATIONS) && (animations_loaded & (1<<index))) {// Make sure that index exists and that an animation has been loaded to the index

        animations[index].running = false;
    }
}

void animation_service (void) {
    for (int i = 0; i < ANIMATION_NUM_ANIMATIONS; i++) {
        if ((animations_loaded & (1<<i)) && animations[i].running) { // verify that an animation has been loaded to the index

            if (millis >= animations[i].next_frame_time) {
                if (animations[i].current_frame >= animations[i].length) {
                    animations[i].current_frame = 0;
                }
                
                lcd_write_char(animations[i].frames[animations[i].current_frame], animations[i].location);
//                lcd_write_int(animations[i].current_frame, LCD_LOCATION_CURSOR);
                
                animations[i].next_frame_time =  millis + (uint8_t)(animations[i].frame_delay);
                
                animations[i].current_frame++;
            }
        }
    }
}

