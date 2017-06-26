//
//  submasters.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-11-11.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#ifndef submasters_h
#define submasters_h

#define SUBMASTER_NUM       8           // Number of submasters that can be stored locally
#define SUBMASTER_CHANS     80          // Number of channel value pairs that can be stored in each submaster


// MARK: Function Definitions


#endif /* submasters_h */


/* Anatomy of a submaster:
 *
 * |Channels|-|<------------------Config------------------>|
 * |<-240B->|-|76543210|76543210|76543210|76543210|76543210|
 *             CNFG0    CNFG1    CNFG2    CNFG3    CNFG4
 *
 * Channel:
 * |<--Address-->||Value|-|<--Address-->||Value|-...80 addresses
 * |<--2 Bytes-->||1Byte|-|<--2 Bytes-->||1Byte|-...80 addresses
 *
 * Config:
 *      CNFG07: Enabled
 *      
 *      CNFG2:  Fade up time
 *      CNFG3:  Fade down time
 *      CNFG4:  Bump Level
 *
 *
 */
