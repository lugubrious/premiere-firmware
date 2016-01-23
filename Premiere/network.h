//
//  network.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-11-07.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#ifndef network_h
#define network_h

#include "global.h"

// MARK: Functions
/**
 * Initializes the SPI interface and the network interface using DHCP
 */
extern void network_init(void);

/**
 * Deinitilaizes network interface
 */
extern void network_deinit(void);

/**
 * To be run every main loop
 */
extern void network_service(void);

/**
 * Returns the current IP address
 *
 * @return The current IP address
 */
extern void network_get_ip_addr(uint8_t *address);

/*
 * Sends updated ADC values to control app
 *
 * @param values A list of ADC values. Must be eight bytes long.
 * @return 0 is data was sent, -1 if data could not be sent (no UDP connection)
 */
extern int network_push_adc_update (const uint8_t* values);

/*
 * Sends an updated list of button events that have yet to be marked as processed
 *
 * @return 0 is data was sent, -1 if data could not be sent (no UDP connection)
 */
extern int network_push_buttons_update (uint32_t button_values);

#endif /* network_h */
