#pragma once
#include <rrcan.h>

#define UP_BUTTON 4
#define DOWN_BUTTON 2
#define CLUTCH_LEFT 26
#define CLUTCH_RIGHT 27

#ifdef PIO_UNIT_TESTING

#define PIN_NEOPIXEL 21
#define NEOPIXEL_POWER 20

#endif

const uint32_t GREEN = 0x008c00;
const uint32_t ORANGE = 0xffa500;
const uint32_t RED = 0xff0000;

typedef enum {
    UP = SWC_BUTTONS_SHIFT_DIRECTION_UP_CHOICE,
    DOWN = SWC_BUTTONS_SHIFT_DIRECTION_DOWN_CHOICE
} ShiftDirection;
