//
//  font.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-18.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//
//  Custom characters created with http://www.quinapalus.com/hd44780udg.html
//

#ifndef font_h
#define font_h

#define FONT_NUM_CHARS 8

/*
const uint8_t FONT[8][8] = {
    {0,0,10,17,10,4,0,0},
    {0,4,8,17,10,4,0,0},
    {0,4,10,16,10,4,0,0},
    {0,4,10,17,8,4,0,0},
    {0,4,10,17,10,0,0,0},
    {0,4,10,17,2,4,0,0},
    {0,4,10,1,10,4,0,0},
    {0,4,2,17,10,4,0,0}
};*/

const uint8_t FONT[8][8] = {
    {0,16,8,4,2,1,0, 0},        // Char 1: Backslash '\'
    
    {0, 0, 0, 0, 0, 0, 0,31},   // Char 2: Bottom row filled
    {0, 0, 0, 0, 0, 0,31,31},   // Char 3: Two rows filed
    {0, 0, 0, 0, 0,31,31,31},   // Char 4: Three rows filed
    {0, 0, 0, 0,31,31,31,31},   // Char 5: Half filled
    {0, 0, 0,31,31,31,31,31},   // Char 6: Five rows filled
    {0, 0,31,31,31,31,31,31},   // Char 7: All but two rows filled
    {0,31,31,31,31,31,31,31}    // Char 8: All but one rwo filled
};

const uint8_t FONT_PERCENTAGE_INDICATOR[9] = {254, 2, 3, 4, 5, 6, 7, 8, 255}; // 254 = empty, 255 = full

#endif /* font_h */
