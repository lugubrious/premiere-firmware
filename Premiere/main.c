//
//  Main.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-01.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include <avr/io.h>
#include <avr/interrupt.h>

#include "stdlib.h"

#include "global.h"
#include "pindefinitions.h"
#include "shift.h"
#include "dmx.h"
#include "lcd.h"
#include "animation.h"
#include "strings.h"
#include "buttons.h"
#include "network.h"

#include "./libethernet/ethernet.h"
#include "./libethernet/enc28j60.h"

// MARK: Function prototypes
void main_loop(void);
uint8_t adcs_are_dirty(void);

// MARK: Variables Definitions
volatile uint32_t millis;

static volatile uint8_t adc_current_chan;
static volatile uint8_t prev_adc_values[8];
volatile uint8_t adc_values[8];

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
    TCCR0B |= (1 << CS01) | (1 << CS00);            // set prescaler to 64 and start timer 0
}

void init_adc(void) {
    ADMUX |= (1<<REFS0)|(1 << ADLAR);               // Refrence = AVCC with external capacitor at AREF pin, left adjusted
    ADCSRA |= (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // Enable ADC and set prescaler to 128 (125KHz), enable ADC interupts
    DIDR0 |= 0xFF;                                  // Disable digital inputs on analog pins to save power
    ADMUX &= 0xF8;                                  // Make sure that channel 0 is selected
}


int main(void) {
    cli();                                          // Clear(disable) interupts
    
    init_io();

    init_timers();
    init_adc();
    
    dmxInit_universe_one();                         // Initialize universe one
    dmx_init_universe_two();                        // Initialize universe two
    
    lcd_init();                                     // Initilize the lcd with the default settings in lcd.c (which are conveniantly the settings we want)
    
    sei();                                          // Enable interupts

    /*
    // Make sure stage doesn't go black
    for (int i = 58; i < 74; i++) {
        dmx_set_dimmer(i, 255);
    }
    
    dmx_set_dimmer(94, 255);
    dmx_set_dimmer(90, 255);
    dmx_set_dimmer(86, 255);
    dmx_set_dimmer(85, 255);
    dmx_set_dimmer(82, 255);
    dmx_set_dimmer(77, 255);
    dmx_set_dimmer(12, 255);
    
    for (int i = 0; i < 11; i++) {
        dmx_set_dimmer(i, 255);
    }
    
    dmx_set_dimmer(12, 255);
    dmx_set_dimmer(15, 255);
    dmx_set_dimmer(18, 255);
    dmx_set_dimmer(23, 255);*/
    
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
    
//    struct Animation loading_animation = animation_create_new(ANIMATION_LOADING_LENGTH, ANIMATION_LOADING_FRAMERATE, LCD_LINE_ONE_START, ANIMATION_LOADING_CHARS);
//    animation_load(loading_animation, 0);
//    animation_start(0);
    
    lcd_write_string("Network Setup...", LCD_LINE_ONE_START);
    
    network_init();
    
    // Write the initial menu to the display
    char buffer[18];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(strings_main_menus[0])));   // Get string from program memory. Uses pgm_read_word to get a pointer to the string in program space
    lcd_write_string(buffer, LCD_LINE_ONE_START);
    
    uint8_t ip[4];
    network_get_ip_addr(ip);
    lcd_write_int(ip[0], 3, LCD_LINE_ONE_START);
    lcd_write_char('.', LCD_LINE_ONE_START + 3);
    lcd_write_int(ip[1], 3, LCD_LINE_ONE_START + 4);
    lcd_write_char('.', LCD_LINE_ONE_START + 7);
    lcd_write_int(ip[2], 3, LCD_LINE_ONE_START + 8);
    lcd_write_char('.', LCD_LINE_ONE_START + 11);
    lcd_write_int(ip[3], 3, LCD_LINE_ONE_START + 12);
    
    for (;;) {
        main_loop();                                // Execute main loop forevermore
    }
    
    network_deinit();
}

void main_loop(void) {
    // Run main loop funcions for various systems
    dmx_service();
    animation_service();

    // Get the latest data from the buttons
    if (shift_in()) {
        buttons_sections_dirty |= (1<<BUTTON_KEYS_DIRTY);
    }
    button_service();

    
    // Check for dirty flags/new input and take any needed actions
    uint8_t adc_is_dirty = adcs_are_dirty();
    if (adc_is_dirty != 0) {
        network_push_adc_update(adc_values);
    }
    
    if ((buttons_keypad_dirty != 0) || (buttons_bump_dirty != 0)) {
        network_push_buttons_update(buttons_keypad_dirty | ((uint32_t)buttons_bump_dirty << 24));
        buttons_keypad_dirty = 0;
        buttons_bump_dirty = 0;
    }

 /*  Hard coded tests...
    if (adc_is_dirty & (1<<1)) {
        dmx_set_dimmer(512, adc_values[1]);
        adc_is_dirty &= ~(1<<1);
    }
    
    if (adc_is_dirty & (1<<2)) {
        dmx_set_dimmer(513, adc_values[2]);
        adc_is_dirty &= ~(1<<2);
    }
    
    if (adc_is_dirty & (1<<3)) {
        dmx_set_dimmer(514, adc_values[3]);
        adc_is_dirty &= ~(1<<3);
    }
    
    if (adc_is_dirty & (1<<4)) {
        dmx_set_dimmer(515, adc_values[4]);
        adc_is_dirty &= ~(1<<4);
    }
    
    if (adc_is_dirty & (1<<6)) {
        dmx_set_dimmer(96, adc_values[6]);
        adc_is_dirty &= ~(1<<6);
    }
    
    if (adc_is_dirty & (1<<7)) {
        dmx_set_dimmer(95, adc_values[7]);
        dmx_set_dimmer(39, adc_values[7]);
        adc_is_dirty &= ~(1<<7);
    }*/
    
    // Check for new packets
    network_service();
    
    // Update outputs
    shift_out();
}

uint8_t adcs_are_dirty (void) {
    static bool not_first_run;                      // Keep track of the first run since when the program first runs the current and prev values will probably be 0, but we want to make sure that anything involving the adc is initilzed
    if (__builtin_expect(!not_first_run, 0)) {      // __builtin_expect tells the branch predictor that we expect the outcome of !not_first_run to be 0, as it will be in everycase except for the first run
        not_first_run = true;
        return 0xFF;
    } else {
        uint8_t ret = 0;
        for (int i = 0; i < 8; i++) {
            ret |= ((prev_adc_values[i] != adc_values[i]) << i);
            prev_adc_values[i] = adc_values[i];
        }
        return ret;
    }
}

// MARK: Interupt Service Routines

ISR (TIMER0_COMPA_vect) {                           // timer0 comp (called every millisecond)
    millis++;
}

ISR (ADC_vect) {                                    // ADC conversion complete
    uint8_t value = ADCH;                           // Get ADC value. The value is left adjusted for easy 8-bit reading(we don't have to shift it over). We just drop that last two bits to avoid ripple as we really don't need ten bit precision.

    adc_values[adc_current_chan] = value;
    
    // Get ready for next conversion
    adc_current_chan++;                             // Increment ADC channel
    adc_current_chan &= 7;                          // Make sure ADC channel only uses first 3 bits (less than 8)
    ADMUX &= 0xF8;                                  // Set adc channel to 0
    ADMUX |= adc_current_chan;                      // Set adc channel
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