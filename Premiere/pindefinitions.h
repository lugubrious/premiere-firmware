//
//  pinDefinitions.h
//  Premiere
//
//  Created by Samuel Dewan on 2015-10-01.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#ifndef pinDefinitions_h
#define pinDefinitions_h

// MARK: Shift out
#define SHIFT_OUT_DATA_DDR  DDRB
#define SHIFT_OUT_DATA_PORT PORTB
#define SHIFT_OUT_DATA_NUM  PINB0

#define SHIFT_OUT_SCLK_DDR  DDRB
#define SHIFT_OUT_SCLK_PORT PORTB
#define SHIFT_OUT_SCLK_NUM  PINB1

#define SHIFT_OUT_RCLK_DDR  DDRB
#define SHIFT_OUT_RCLK_PORT PORTB
#define SHIFT_OUT_RCLK_NUM  PINB3

// MARK: Shift in
#define SHIFT_IN_LOAD_DDR   DDRD
#define SHIFT_IN_LOAD_PORT  PORTD
#define SHIFT_IN_LOAD_NUM   PORTD5

#define SHIFT_IN_DATA_DDR   DDRD
#define SHIFT_IN_DATA_PORT  PORTD
#define SHIFT_IN_DATA_PIN   PIND
#define SHIFT_IN_DATA_NUM   PIND0

#define SHIFT_IN_CLK_DDR    DDRD
#define SHIFT_IN_CLK_PORT   PORTD
#define SHIFT_IN_CLK_NUM    PORTD4

// MARK: Bump Buttons
#define BUMP_ONE_DDR        DDRC
#define BUMP_ONE_PORT       PORTC
#define BUMP_ONE_PIN        PINC
#define BUMP_ONE_NUM        PINC7
#define BUMP_ONE_ID         0

#define BUMP_TWO_DDR        DDRC
#define BUMP_TWO_PORT       PORTC
#define BUMP_TWO_PIN        PINC
#define BUMP_TWO_NUM        PINC6
#define BUMP_TWO_ID         1

#define BUMP_THREE_DDR      DDRC
#define BUMP_THREE_PORT     PORTC
#define BUMP_THREE_PIN      PINC
#define BUMP_THREE_NUM      PINC5
#define BUMP_THREE_ID       2

#define BUMP_FOUR_DDR       DDRC
#define BUMP_FOUR_PORT      PORTC
#define BUMP_FOUR_PIN       PINC
#define BUMP_FOUR_NUM       PINC4
#define BUMP_FOUR_ID        3

#define BUMP_FIVE_DDR       DDRC
#define BUMP_FIVE_PORT      PORTC
#define BUMP_FIVE_PIN       PINC
#define BUMP_FIVE_NUM       PINC3
#define BUMP_FIVE_ID        4

#define BUMP_SIX_DDR        DDRC
#define BUMP_SIX_PORT       PORTC
#define BUMP_SIX_PIN        PINC
#define BUMP_SIX_NUM        PINC2
#define BUMP_SIX_ID         5

#define BUMP_SEVEN_DDR      DDRD
#define BUMP_SEVEN_PORT     PORTD
#define BUMP_SEVEN_PIN      PIND
#define BUMP_SEVEN_NUM      PIND7
#define BUMP_SEVEN_ID       6

#define BUMP_EIGHT_DDR      DDRD
#define BUMP_EIGHT_PORT     PORTD
#define BUMP_EIGHT_PIN      PIND
#define BUMP_EIGHT_NUM      PIND6
#define BUMP_EIGHT_ID       7

#define NUM_BUMP_BUTTONS    8


// MARK: Key pad
#define KEY_0_ID            7
#define KEY_1_ID            6
#define KEY_2_ID            5
#define KEY_3_ID            4
#define KEY_4_ID            3
#define KEY_5_ID            2
#define KEY_6_ID            1
#define KEY_7_ID            0

#define KEY_8_ID            15
#define KEY_9_ID            14
#define KEY_F1_ID           13
#define KEY_F2_ID           12
#define KEY_F3_ID           11
#define KEY_F4_ID           10
#define KEY_THRU_ID         9
#define KEY_EXCEPT_ID       8

#define KEY_AND_ID          20//23
#define KEY_AT_ID           21//22
#define KEY_CLEAR_ID        22//21
#define KEY_ENTER_ID        23//20
#define KEY_BACK_ID         19//19

#define NUM_KEYS            32

/*
    7 (0)       - 7
    6 (1)       - 6
    5 (0)       - 5
    4 (1)       - 4
    3 (0)       - 3
    2 (1)       - 2
    1 (0)       - 1
    0 (1)       - 0
    8 (-)       - 8
    9 (<)       - 9
    10 (F4)     - 10
    11 (F3)     - 11
    12 (F2)     - 12
    13 (F1)     - 13
    14 (9)      - 14
    15 (8)      - 15
    16 (-)      - -
    17 (-)      - -
    18 (-)      - -
    19 (Back)   - 15
    20 (*)      - 15
    21 (Clear)  - 15
    22 (@)      - 15
    23 (+)      - 15
 */

/*
    +       - 3D
    @       - 3C
    Clear   - 3B
    *       - 3A
    Back    - 3
 */

/*
    S1      S2      S3      S4
    7       8       9       CLEAR
    4       5       6       AND
    1       2       3       EXCEPT
    THRU    0       AT      ENTER
 */

#define KEY_GO_DDR          DDRB
#define KEY_GO_PORT         PORTB
#define KEY_GO_PIN          PINB
#define KEY_GO_NUM          PINB2
#define KEY_GO_ID           16

#define KEY_STOP_DDR        DDRD
#define KEY_STOP_PORT       PORTD
#define KEY_STOP_PIN        PIND
#define KEY_STOP_NUM        PIND2
#define KEY_STOP_ID         17

#define PLAYBACK_START      NUM_KEYS
#define PLAYBACK_NUM        2

// MARK: Shift out
#define LED_BUMP_ONE_ID     15
#define LED_BUMP_TWO_ID     14
#define LED_BUMP_THREE_ID   13
#define LED_BUMP_FOUR_ID    12
#define LED_BUMP_FIVE_ID    11
#define LED_BUMP_SIX_ID     10
#define LED_BUMP_SEVEN_ID   9
#define LED_BUMP_EIGHT_ID   8

#define LED_GO_ID           6
#define LED_STOP_ID         5
#define LED_AT_ID           4
#define LED_THRU_ID         3
#define LED_EXCEPT_ID       2
#define LED_AND_ID          1

#endif /* pinDefinitions_h */
