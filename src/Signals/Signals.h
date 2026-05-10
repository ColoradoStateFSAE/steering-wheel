#pragma once
#include <functional>
#include "constants.h"

struct Signals {
    bool offline = false;

    float clutchLeft = 0;
    float clutchRight = 0;

    float clutchLeftRaw = 0;
    float clutchRightRaw = 0;

    float rotaryLeftVolts = 0;
    float rotaryRightVolts = 0;

    std::function<void(ShiftDirection)> onButton = [](ShiftDirection) {};
    std::function<void(bool, int)> onEncoder = [](bool, int) {};
};
