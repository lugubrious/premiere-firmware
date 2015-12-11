//
//  Main.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-01.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include <avr/io.h>
#include <avr/interrupt.h>

#include "global.h"
#include "pindefinitions.h"
#include "shift.h"
#include "dmx.h"
#include "lcd.h"
#include "animation.h"
#include "strings.h"
#include "buttons.h"
#include "network.h"

// MARK: Constants


// MARK: Function prototypes
void main_loop(void);


// MARK: Variables Definitions
volatile uint32_t millis, last_beat;
static volatile uint32_t key_go_last_pressed, key_stop_last_pressed;

static volatile uint8_t bump_button_is_dirty, adc_current_chan, adc_is_dirty = 0xFF;
static volatile uint8_t adc_values[8];

// MARK: Funciton definitions
void init_io(void) {
	// Shift in
    SHIFT_OUT_DATA_DDR |= (1<<SHIFT_OUT_DATA_NUM);  // Set shift out data pin as an output
    SHIFT_OUT_SCLK_DDR |= (1<<SHIFT_OUT_SCLK_NUM);  // Set shift out sclk pin as an output
    SHIFT_OUT_RCLK_DDR |= (1<<SHIFT_OUT_RCLK_NUM);  // Set shift out rclk pin as an output
    // Shift out
    SHIFT_IN_DATA_DDR &= ~(1<<SHIFT_IN_DATA_NUM);   // Set shift in data pin as an input
    SHIFT_IN_DATA_PORT &= ~(1<<SHIFT_IN_DATA_NUM);  // Make sure pullup on shift in data pin is diasbled
    SHIFT_IN_LOAD_DDR |= (1<<SHIFT_IN_LOAD_NUM);    // Set shift in latch pin as an output
    SHIFT_IN_CLK_DDR |= (1<<SHIFT_IN_CLK_NUM);      // Set shift in clk pin as an output
    // Bump buttons
    BUMP_ONE_DDR &= ~(1<<BUMP_ONE_NUM);             // Set the bump button one pin as an input
    BUMP_ONE_PORT |= (1<<BUMP_ONE_NUM);             // Enable pullup on bump button one pin
    BUMP_TWO_DDR &= ~(1<<BUMP_TWO_NUM);             // Set the bump button two pin as an input
    BUMP_TWO_PORT |= (1<<BUMP_TWO_NUM);             // Enable pullup on bump button two pin
    BUMP_THREE_DDR &= ~(1<<BUMP_THREE_NUM);         // Set the bump button three pin as an input
    BUMP_THREE_PORT |= (1<<BUMP_THREE_NUM);         // Enable pullup on bump button three pin
    BUMP_FOUR_DDR &= ~(1<<BUMP_FOUR_NUM);           // Set the bump button four pin as an input
    BUMP_FOUR_PORT |= (1<<BUMP_FOUR_NUM);           // Enable pullup on bump button four pin
    BUMP_FIVE_DDR &= ~(1<<BUMP_FIVE_NUM);           // Set the bump button five pin as an input
    BUMP_FIVE_PORT |= (1<<BUMP_FIVE_NUM);           // Enable pullup on bump button five pin
    BUMP_SIX_DDR &= ~(1<<BUMP_SIX_NUM);             // Set the bump button six pin as an input
    BUMP_SIX_PORT |= (1<<BUMP_SIX_NUM);             // Enable pullup on bump button six pin

    BUMP_SEVEN_DDR &= ~(1<<BUMP_SEVEN_NUM);         // Set the bump button seven pin as an input
    BUMP_SEVEN_PORT |= (1<<BUMP_SEVEN_NUM);         // Enable pullup on bump button seven pin
    BUMP_EIGHT_DDR &= ~(1<<BUMP_EIGHT_NUM);         // Set the bump button eight pin as an input
    BUMP_EIGHT_PORT |= (1<<BUMP_EIGHT_NUM);         // Enable pullup on bump button eight pin
    
    // Key pad
    KEY_GO_DDR &= ~(1<<KEY_GO_NUM);                 // Set the go button pin as an input
    KEY_GO_PORT |= (1<<KEY_GO_NUM);                 // Enable pullup on go button pin
    KEY_STOP_DDR &= ~(1<<KEY_STOP_NUM);             // Set the stop/back button pin as an input
    KEY_STOP_PORT |= (1<<KEY_STOP_NUM);             // Enable pullup on stop/back button pin
    
    // Interupts
    EICRA |= (1<<ISC21)|(1<<ISC01);                 // Set intrupts 2(go) and 0(stop) to falling edge
    EIMSK |= (1<<INT2)|(1<INT0);                    // Enable external interupts for go and stop buttons
    PCICR |= (1<<PCIE3)|(1<<PCIE2);                 // Enable pin change interupts for PCINT[24,32] and PCINT[16,23]
    PCMSK3 = 0xC0;                                  // Enable pin change interupts for PCINT30 and PCINT31
    PCMSK2 = 0xFC;                                  // Enable pin change interupts for PCINT[18,23]
}

void init_timers(void) {
    //Timer 0 (clock)
    TCCR0A |= (1 << WGM01);                         // Set the Timer Mode to CTC
    OCR0A = 250;                                    // Set the value to count to 250 (1 millisecond at 16Mhz where prescaler = 64)
    TIMSK0 |= (1 << OCIE0A);                        // Set the ISR COMPA vector
    TCCR0B |= (1 << CS01) | (1 << CS00);            // set prescaler to 8 and start timer 0
}

void init_adc(void) {
    ADMUX |= (1<<REFS0)|(1 << ADLAR);               // Refrence = AVCC with external capacitor at AREF pin, left adjusted
    ADCSRA |= (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // Enable ADC and set prescaler to 128 (125KHz), enable ADC interupts
    ADMUX &= 0xF8;                                  // Make sure that channel 0 is selected
}


int main(void) {
    cli();                                          // Clear(disable) interupts
    
    init_io();
//    BUMP_EIGHT_DDR |= (1<<BUMP_EIGHT_NUM);
//    BUMP_EIGHT_PORT |= (1<<BUMP_EIGHT_NUM);
//    BUMP_SEVEN_DDR |= (1<<BUMP_SEVEN_NUM);

    init_timers();
    init_adc();
    
    dmxInit_universe_one();                         // Initialize universe one
    dmx_init_universe_two();                        // Initialize universe two
    
    lcd_init();                                     // Initilize the lcd with the default settings in lcd.c (which are conveniantly the settings we want~ I wonder how that happend?)
    
    sei();                                          // Enable interupts
    
    dmx_one_start_frame();                          // Start sending frames on universe one
    dmx_two_start_frame();                          // Start sending frames on universe two
    
    ADCSRA |= (1<<ADSC);                            // Start first ADC conversion
    
#ifdef DEBUG_MODE
    shift_out_buffer = 0b1111111111111111;          // Test the LEDs
    shift_out();                                    // Make sure the shift out regs have a defined state
    shift_out_buffer = 0;                           // Next time shift out is called outputs return to off
# else
    shift_out();                                    // Clear the shift registers (they start in an undifined state, so some LEDs are probably on)
#endif
    
    struct Animation loading_animation = animation_create_new(ANIMATION_LOADING_LENGTH, ANIMATION_LOADING_FRAMERATE, LCD_LINE_ONE_START, ANIMATION_LOADING_CHARS);
    animation_load(loading_animation, 0);
    animation_start(0);
    
//    network_init();
    
    for (;;) {
        main_loop();                                // Execute main loop forevermore
    }
    
    network_deinit();
}

void main_loop(void) {
//    if ((millis - last_beat) >= 1000) {
//        last_beat = millis;
//        shift_out_buffer ^= (1<<8);
//    }

    // Run main loop funcions for various systems
    dmx_service();
    animation_service();
    
    // Get the latest data from the buttons
    if (shift_in()) {
        buttons_sections_dirty |= (1<<BUTTON_KEYS_DIRTY);
    }
    button_service();

    if (buttons_keypad_dirty & ((uint32_t)1<<KEY_1_ID)) {
        shift_out_buffer ^= (1<<15);
        buttons_keypad_dirty &= ~((uint32_t)1<<KEY_1_ID);
    }
    
    if (buttons_keypad_dirty & ((uint32_t)1<<KEY_GO_ID)) {
        shift_out_buffer ^= (1<<14);
        buttons_keypad_dirty &= ~((uint32_t)1<<KEY_GO_ID);
    }

    if (buttons_bump_dirty & ((uint32_t)1<<BUMP_ONE_ID)) {
        shift_out_buffer ^= (1<<13);
        buttons_bump_dirty &= ~((uint32_t)1<<BUMP_ONE_ID);
    }
    
    lcd_write_int(shift_in_buffer, 12, LCD_LINE_TWO_START);
    
    // Check for dirty flags/new input and take any needed actions
    if ((adc_is_dirty & (1<<0))) {
        lcd_write_int(adc_values[0], 3, LCD_LINE_ONE_START + 13);
        lcd_write_percentage(adc_values[0], LCD_LINE_ONE_START + 11);
        adc_is_dirty &= ~(1<0);
    }
    
    // Check for new packets
//    network_service();
    
    // Update outputs
    shift_out();
}


// MARK: Interupt Service Routines

ISR (TIMER0_COMPA_vect) {                           // timer0 overflow (called every 200 microseconds)
    millis++;
}

ISR (ADC_vect) {                                    // ADC conversion complete
    uint8_t value = ADCH;                           // Get ADC value. The value is left adjusted for easy 8-bit reading(we don't have to shift it over). We just drop that last two bits to avoid ripple as we really don't need ten bit precision.
    
    if (value != adc_values[adc_current_chan]) {    // Check if value has changed sicne last read
        adc_is_dirty |= (1<<adc_current_chan);      // If the value has changed set a flag so that the change can be addressed in the next loop
    }
    
    adc_values[adc_current_chan] = value;
    
    // Get ready for next conversion
    adc_current_chan++;                             // Increment ADC channel
    adc_current_chan &= 7;                          // Make sure that ADC channel is less than 8
    ADCSRA |= (1<<ADSC);                            // Start the next ADC conversion
}

ISR (INT0_vect) {                                   // Stop/back button falling edge
    buttons_sections_dirty |= (1<<BUTTON_PLAYBACKS_DIRTY);
}

ISR (INT2_vect) {                                   // Go button falling edge
    buttons_sections_dirty |= (1<<BUTTON_PLAYBACKS_DIRTY);
}

ISR (PCINT2_vect) {                                 // Pin change on one of bump buttons 1 to 6
    buttons_sections_dirty |= (1<<BUTTON_BUMPS_DIRTY);
}

ISR (PCINT3_vect) {                                 // Pin change on bump button 7 or 8
    buttons_sections_dirty |= (1<<BUTTON_BUMPS_DIRTY);
}