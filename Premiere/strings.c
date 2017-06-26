//
//  strings.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-12-19.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "strings.h"

// MARK: Main Menus
//                              Line 1:                2:                end
const char strings_menu_0[] PROGMEM = "                \n                ";
const char strings_menu_1[] PROGMEM = "Fixture: **    *\n UP  DWN  <   > ";
const char strings_menu_2[] PROGMEM = "Submaster: **  *\n         <   > ";
const char strings_menu_3[] PROGMEM = "Options:        \nDiag Net  <   > ";

PGM_P const strings_main_menus[] PROGMEM = {
    strings_menu_0,
    strings_menu_1,
    strings_menu_2,
    strings_menu_3
};


// MARK: Network Submenus
//                                  Line 1:                2:                end
const char strings_net_menu_0[] PROGMEM = "***.***.***.*** \n             ^ ";

PGM_P const strings_net_menus[] PROGMEM = {
    strings_net_menu_0
};

// const fish* const