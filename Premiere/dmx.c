//
//  DMX.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-01.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "dmx.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "shift.h"

// MARK: Enum Definitions
typedef enum {DISABLED, BREAK, STARTBYTE, SLOTS, IDLE} stage;
// DISABLED:        DMX not being transmitted
// BREAK:           Break and MAB need to be sent
// BREAKSENT:       Break and MAB are being sent
// STARTBYTE:       Start byte needs to be sent
// STARTBYTESENT:   Start byte is being sent
// SLOTS:           Slots are being sent
// IDLE:            Waiting to send next packet

// MARK: Variable Definitions
volatile uint8_t dmx_universe_one[DMX_NUM_SLOTS];
volatile uint8_t dmx_universe_two[DMX_NUM_SLOTS];

static uint32_t dmxOneLastFrame, dmxTwoLastFrame;   // The value of millis when a frame was last sent
static volatile stage dmxOneStage, dmxTwoStage;     // DMX stages


// Mark: Function Definitions

void dmxInit_universe_one(void) {
    // It is assumed that the global interupt flag is cleared when this function runs
    
    // USART configuration
    UBRR0H = 0;                                     // Set baud rate to 76.8kbaud (at 16mhz clock)
    UBRR0L = 12;
    UCSR0B = (1<<TXEN0)|(1<<TXCIE0);                // Enable transmitter only, TX done interupt enabled
    UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);    // Set frame format: 8data, 2stop bit
    
    dmxOneStage = IDLE;
}

void dmx_init_universe_two(void) {
    // It is assumed that the global interupt flag is cleared when this function runs
    
    // USART configuration
    UBRR1H = 0;                                     // Set baud rate to 76.8kbaud (at 16mhz clock)
    UBRR1L = 12;
    UCSR1B = (1<<TXEN1)|(1<<TXCIE1);                // Enable transmitter only, TX done interupt enabled
    UCSR1C = (1<<USBS1)|(1<<UCSZ10)|(1<<UCSZ11);    // Set frame format: 8data, 2stop bit
    
    dmxTwoStage = IDLE;
}

void dmx_service (void) {
    // Universe One
    if ((dmxOneStage == IDLE) && ((millis - dmxOneLastFrame) > DMX_MAX_FRAME_FREQUENCY)) {
        dmx_one_start_frame();
        dmxOneLastFrame = millis;
    }
    // Universe Two
    if ((dmxTwoStage == IDLE) && ((millis - dmxTwoLastFrame) > DMX_MAX_FRAME_FREQUENCY)) {
        dmx_two_start_frame();
        dmxTwoLastFrame = millis;
    }
}

void dmx_one_start_frame(void) {
    dmxOneStage = BREAK;
    dmxOneLastFrame = millis;
    UBRR0H = 0;                                     // Set baud rate to 76.8kbaud (at 16mhz clock)
    UBRR0L = 12;
    UDR0 = 0;                                       // Transmit a Zero
    // At 76.8kbaud this will hold the line low for 104µs (break) then high (the stop bits) for 26µs (MAB)
}

void dmx_two_start_frame(void) {
    dmxTwoStage = BREAK;
    dmxTwoLastFrame = millis;
    UBRR1H = 0;                                     // Set baud rate to 76.8kbaud (at 16mhz clock)
    UBRR1L = 12;
    UDR1 = 0;                                       // Transmit a Zero
    // At 76.8kbaud this will hold the line low for 104µs (break) then high (the stop bits) for 26µs (MAB)
}

void dmx_stop_universe_one(void) {
    dmxOneStage = DISABLED;
}

void dmx_stop_universe_two(void) {
    dmxTwoStage = DISABLED;
}

void dmx_set_dimmer(uint16_t dimmer, uint8_t value) {
    if (dimmer < DMX_UNIVERSE_LENGTH) {
        dmx_universe_one[dimmer] = value;
    } else {
        dimmer -= DMX_UNIVERSE_LENGTH;
        dmx_universe_two[dimmer] = value;
    }
}

uint8_t dmx_get_dimmer(uint16_t dimmer) {
    if (dimmer < DMX_UNIVERSE_LENGTH) {
        return dmx_universe_one[dimmer];
    } else {
        dimmer -= DMX_UNIVERSE_LENGTH;
        return dmx_universe_two[dimmer];
    }
}


// MARK: Interupt Service Routines
ISR (USART0_TX_vect) {                              // Transmit finished on USART0
    static uint16_t currentSlot;
    switch (dmxOneStage) {
        case DISABLED:                              // This shouldn't happen
            break;
        case BREAK:                                 // The break is finished sending
            dmxOneStage = STARTBYTE;
            UBRR0H = 0;                             // Set baud rate (250Kbaud at 16mhz clock)
            UBRR0L = 3;
            UDR0 = DMX_START_BYTE;                  // Send start byte
            break;
        case STARTBYTE:                             // The start byte is finished sending
            dmxOneStage = SLOTS;
            currentSlot = 1;                        // get ready to send next slot
            UDR0 = dmx_universe_one[0];             // Sent first Slot
            break;
        case SLOTS:                                 // The latest slot is finished sending
            if (currentSlot < DMX_NUM_SLOTS) {      // if there are slots left to send
                UDR0 = dmx_universe_one[currentSlot];
                currentSlot++;
            } else {
                dmxOneStage = IDLE;                 // packet is done!
            }
            break;
        case IDLE:                                  // The last slot is finished sending
            break;
    }
}

ISR (USART1_TX_vect) {                              // Transmit finished on USART1
    static uint16_t currentSlot;
    switch (dmxTwoStage) {
        case DISABLED:                              // This shouldn't happen
            break;
        case BREAK:                                 // The break is finished sending
            dmxTwoStage = STARTBYTE;
            UBRR1H = 0;                             // Set baud rate (250Kbaud at 16mhz clock)
            UBRR1L = 3;
            UDR1 = DMX_START_BYTE;                  // Send start byte
            break;
        case STARTBYTE:                             // The start byte is finished sending
            dmxTwoStage = SLOTS;
            currentSlot = 1;                        // get ready to send next slot
            UDR1 = dmx_universe_two[0];             // Sent first Slot
            break;
        case SLOTS:                                 // The latest slot is finished sending
            if (currentSlot < DMX_NUM_SLOTS) {      // if there are slots left to send
                UDR1 = dmx_universe_two[currentSlot];
                currentSlot++;
            } else {
                dmxTwoStage = IDLE;                 // packet is done!
            }
            break;
        case IDLE:                                  // The last slot is finished sending
            break;
    }
}