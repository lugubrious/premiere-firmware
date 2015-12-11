//
//  network.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-11-07.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#ifndef network_h
#define network_h

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

#endif /* network_h */
