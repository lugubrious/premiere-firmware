//
//  EEPROM.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-11-11.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#ifndef eeprom_h
#define eeprom_h

// MARK: EEPROM Addresses

// We have 2KiB of eeprom, I though that was a lot until I actually figured our how much space submasters take up.

// Bytes 0-79 are for configuration
// Bytes 80 - 2039 are for submasters
//  Sub 0 = 80-324
//  Sub 1 = 325+569
//  Sub 2 = 570-814
//  Sub 3 = 815-1059
//  Sub 4 = 1060-1304
//  Sub 5 = 1305-1549
//  Sub 6 = 1550-1794
//  Sub 7 = 1795-2039
// Bytes 2040-2047 are for callibration

// A submast consistes of 8 channel value pairs (two-byte chan, one byte value) and then 5 byte of space of submaster config info
// Addres of sub = EEPROM_SUBMASTER_START+(index*EEPROM_SUBMASTER_LENGTH)
// Addres of chan = sub + (index * EEPROM_SUBMASTER_CHAN_LENGTH)
#define EEPROM_SUBMASTER_START          80
#define EEPROM_SUBMASTER_LENGTH         245
#define EEPROM_SUBMASTER_CHAN_LENGTH    3
#define EEPROM_CONFIG_START             240

#define EEPROM_OSCCAL                   2040

#endif /* eeprom_h */
