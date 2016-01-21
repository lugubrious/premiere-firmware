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

#endif /* network_h */
