#pragma once
#include <rrcan.h>

#define UP_BUTTON 47
#define DOWN_BUTTON 45
#define CLUTCH_LEFT 44
#define CLUTCH_RIGHT 46
#define ENCODER_BUTTON 21
#define ENCODER_A 22
#define ENCODER_B 23
#define CAN_CS 17

const uint32_t RED = 0xff0000;
const uint32_t GREEN = 0x00ff00;
const uint32_t ORANGE = 0xffa500;

typedef enum {
    UP = SWC_BUTTONS_SHIFT_UP_CHOICE,
    DOWN = SWC_BUTTONS_SHIFT_DOWN_CHOICE
} ShiftDirection;
