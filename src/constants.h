#pragma once
#include <rrcan.h>

#define UP_BUTTON 4
#define DOWN_BUTTON 2
#define CLUTCH_LEFT 26
#define CLUTCH_RIGHT 27

#ifdef PIO_UNIT_TESTING

#define NEOPIXEL_POWER 20
#define PIN_NEOPIXEL 21

#endif

#define GREEN 0x008c00
#define ORANGE 0xffa500
#define RED 0xff0000

typedef enum {
    UP = SWC_BUTTONS_SHIFT_DIRECTION_UP_CHOICE,
    DOWN = SWC_BUTTONS_SHIFT_DIRECTION_DOWN_CHOICE
} ShiftDirection;
