//
//  animation.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-27.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#ifndef animation_h
#define animation_h

#include "global.h"

// MARK: Type definitions
struct Animation {
    bool running;
    uint8_t length, frame_delay, current_frame;
    int8_t location;
    const char* frames;
    
    uint32_t next_frame_time;
};

// MARK: Constant definitions
#define ANIMATION_NUM_ANIMATIONS 2

extern const char ANIMATION_LOADING_CHARS[];
extern const double ANIMATION_LOADING_JITTER[];
extern const double ANIMATION_LOADING_FRAMERATE;
extern const uint8_t ANIMATION_LOADING_LENGTH;

// MARK: Global variables


// MARK: Functions
/**
 * Creates a new animation
 * @param length The number of frames in the animation
 * @param frame_rate The number of frames to play per second
 * @param location Where  on the display the character should be drawn
 * @param frames The characters which make up each frame
 * @return The newly made animation
 */
extern struct Animation animation_create_new(uint8_t length, double frame_rate, int8_t location, const char* frames);

/**
 * Puts an animation into on of the animation slots
 * @param ani The animation
 * @param index Which index the animation should be palced in
 */
extern void animation_load (struct Animation ani, uint8_t index);

/**
 * Starts an animation running
 * @param index Which animation should be started
 */
extern void animation_start (uint8_t index);

/**
 * Stops an animation from running
 * @param index Which animation should be stopped
 */
extern void animation_stop (uint8_t index);

/**
 * Updates running animations
 */
extern void animation_service (void);


#endif /* animation_h */
