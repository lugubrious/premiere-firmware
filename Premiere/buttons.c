//
//  buttons.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-11-02.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "buttons.h"

#include "pindefinitions.h"
#include "shift.h"
#include <avr/io.h>

// MARK: Enum Definitions
typedef enum {NONE, DEBOUNCE, PRESSED} stage;
// NONE:        button is not pressed
// DEBOUNCE:    button has been pressed, in the process of verification
// PRESSED:     button press has been verified, ignore the fact that the button is pressed until the button has been released

// MARK: Function declerations
static uint8_t get_bump_buttons (void);
static uint32_t get_keys (void);
static uint32_t get_playback (void);

// MARK: Variable definitions
volatile uint8_t buttons_bump_dirty;
volatile uint32_t buttons_keypad_dirty;

volatile uint8_t buttons_sections_dirty;


static stage buttons_bump_stages[NUM_BUMP_BUTTONS];
static stage buttons_keypad_stages[NUM_KEYS + PLAYBACK_NUM];

static uint32_t buttons_bump_time[NUM_BUMP_BUTTONS];
static uint32_t buttons_keypad_time[NUM_KEYS + PLAYBACK_NUM];

// MARK: Function definitions

void button_service (void) {
    if (buttons_sections_dirty & (1<<BUTTON_BUMPS_DIRTY)) {
        uint8_t bump_buttons_pressed = get_bump_buttons(), not_dirty = 1;
        
        for (int i = 0; i < NUM_BUMP_BUTTONS; i++) {
            if (bump_buttons_pressed & (1<<i)) {            // If the button is pressed
                if (buttons_bump_stages[i] == NONE) {       // If the buttons was previously not pressed
                    buttons_bump_stages[i] = DEBOUNCE;      // Set the stage to debounce
                    buttons_bump_time[i] = millis;          // Record the time that the button was first pressed
                } else if (buttons_bump_stages[i] == DEBOUNCE) {// If the button was pressed previously
                    if (millis >= (buttons_bump_time[i] + BUTTON_DEBOUNCE_TIME)) {  // Check if the button has been held long enough
                        buttons_bump_dirty |= (1<<i);       // Record that the button has been pressed
                        buttons_bump_stages[i] = PRESSED;   // Make sure that the button is ignored until released
                    }
                }
                not_dirty = 0;
            } else {
                buttons_bump_stages[i] = NONE;              // If the button is not pressed make sure that the mode is NONE
            }
        }
        if (not_dirty) {
            buttons_sections_dirty &= ~(1<<BUTTON_BUMPS_DIRTY);
        }
    }
    
    if (buttons_sections_dirty & (1<<BUTTON_KEYS_DIRTY)) {
        uint32_t keys_pressed = get_keys(), not_dirty = 1;

        for (int i = 0; i < NUM_KEYS; i++) {
            if (keys_pressed & ((uint32_t)1<<i)) {          // If the button is pressed
                if (buttons_keypad_stages[i] == NONE) {     // If the buttons was previously not pressed
                    buttons_keypad_stages[i] = DEBOUNCE;    // Set the stage to debounce
                    buttons_keypad_time[i] = millis;        // Record the time that the button was first pressed
                } else if (buttons_keypad_stages[i] == DEBOUNCE) {// If the button was pressed previously

                    if (millis >= (buttons_keypad_time[i] + BUTTON_DEBOUNCE_TIME)) {  // Check if the button has been held long enough
                        buttons_keypad_dirty |= ((uint32_t)1<<i);// Record that the button has been pressed
                        buttons_keypad_stages[i] = PRESSED; // Make sure that the button is ignored until released
                    }
                }
                not_dirty = 0;
            } else {
                buttons_keypad_stages[i] = NONE;            // If the button is not pressed make sure that the mode is NONE
            }
        }
        if (not_dirty) {
            buttons_sections_dirty &= ~(1<<BUTTON_KEYS_DIRTY);
        }
    }
    
    if (buttons_sections_dirty & (1<<BUTTON_PLAYBACKS_DIRTY)) {
        uint32_t keys_pressed = get_playback(), not_dirty = 1;
        
        for (int i = PLAYBACK_START; i < PLAYBACK_START + PLAYBACK_NUM; i++) {
            if (keys_pressed & ((uint32_t)1<<i)) {          // If the button is pressed
                if (buttons_keypad_stages[i] == NONE) {     // If the button was previously not pressed
                    buttons_keypad_stages[i] = DEBOUNCE;    // Set the stage to debounce
                    buttons_keypad_time[i] = millis;        // Record the time that the button was first pressed
                } else if (buttons_keypad_stages[i] == DEBOUNCE) {// If the button was pressed previously
                    if (millis >= (buttons_keypad_time[i] + BUTTON_DEBOUNCE_TIME)) {  // Check if the button has been held long enough
                        buttons_keypad_dirty |= ((uint32_t)1<<i);// Record that the button has been pressed
                        buttons_keypad_stages[i] = PRESSED; // Make sure that the button is ignored until released
                    }
                }
                not_dirty = 0;
            } else {
                buttons_keypad_stages[i] = NONE;            // If the button is not pressed make sure that the mode is NONE
            }
        }
        if (not_dirty) {
            buttons_sections_dirty &= ~(1<<BUTTON_PLAYBACKS_DIRTY);
        }
    }
}

uint8_t get_bump_buttons (void) {
    uint8_t output = 0;
    if (!(BUMP_ONE_PIN & (1<<BUMP_ONE_NUM))) {
        output |= (1<<BUMP_ONE_ID);
    }
    if (!(BUMP_TWO_PIN & (1<<BUMP_TWO_NUM))) {
        output |= (1<<BUMP_TWO_ID);
    }
    if (!(BUMP_THREE_PIN & (1<<BUMP_THREE_NUM))) {
        output |= (1<<BUMP_THREE_ID);
    }
    if (!(BUMP_FOUR_PIN & (1<<BUMP_FOUR_NUM))) {
        output |= (1<<BUMP_FOUR_ID);
    }
    if (!(BUMP_FIVE_PIN & (1<<BUMP_FIVE_NUM))) {
        output |= (1<<BUMP_FIVE_ID);
    }
    if (!(BUMP_SIX_PIN & (1<<BUMP_SIX_NUM))) {
        output |= (1<<BUMP_SIX_ID);
    }
    if (!(BUMP_SEVEN_PIN & (1<<BUMP_SEVEN_NUM))) {
        output |= (1<<BUMP_SEVEN_ID);
    }
    if (!(BUMP_EIGHT_PIN & (1<<BUMP_EIGHT_NUM))) {
        output |= (1<<BUMP_EIGHT_ID);
    }
    return output;
}

uint32_t get_keys () {
    uint32_t output = 0;
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_BACK_ID))) {
        output |= ((uint32_t)1<<KEY_BACK_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_ENTER_ID))) {
        output |= ((uint32_t)1<<KEY_ENTER_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_CLEAR_ID))) {
        output |= ((uint32_t)1<<KEY_CLEAR_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_AT_ID))) {
        output |= ((uint32_t)1<<KEY_AT_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_AND_ID))) {
        output |= ((uint32_t)1<<KEY_AND_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_EXCEPT_ID))) {
        output |= ((uint32_t)1<<KEY_EXCEPT_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_THRU_ID))) {
        output |= ((uint32_t)1<<KEY_THRU_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_F4_ID))) {
        output |= ((uint32_t)1<<KEY_F4_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_F3_ID))) {
        output |= ((uint32_t)1<<KEY_F3_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_F2_ID))) {
        output |= ((uint32_t)1<<KEY_F2_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_F1_ID))) {
        output |= ((uint32_t)1<<KEY_F1_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_9_ID))) {
        output |= ((uint32_t)1<<KEY_9_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_8_ID))) {
        output |= ((uint32_t)1<<KEY_8_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_7_ID))) {
        output |= ((uint32_t)1<<KEY_7_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_6_ID))) {
        output |= ((uint32_t)1<<KEY_6_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_5_ID))) {
        output |= ((uint32_t)1<<KEY_5_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_4_ID))) {
        output |= ((uint32_t)1<<KEY_4_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_3_ID))) {
        output |= ((uint32_t)1<<KEY_3_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_2_ID))) {
        output |= ((uint32_t)1<<KEY_2_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_1_ID))) {
        output |= ((uint32_t)1<<KEY_1_ID);
    }
    if (!(shift_in_buffer & ((uint32_t)1<<KEY_0_ID))) {
        output |= ((uint32_t)1<<KEY_0_ID);
    }
    return output;
}

static uint32_t get_playback (void) {
    uint32_t output = 0;
    if (!(KEY_STOP_PIN & (1<<KEY_STOP_NUM))) {
        output |= ((uint32_t)1<<KEY_STOP_ID);
    }
    if (!(KEY_GO_PIN & (1<<KEY_GO_NUM))) {
        output |= ((uint32_t)1<<KEY_GO_ID);
    }
    return output;
}