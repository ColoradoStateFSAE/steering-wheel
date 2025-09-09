#pragma once
#include <functional>
#include "constants.h"

struct Signals {
    bool offline = true;

    float clutchLeft = 0;
    float clutchRight = 0;

    float clutchLeftRaw = 0;
    float clutchRightRaw = 0;

    std::function<void(ShiftDirection)> onButton = [](ShiftDirection) {};
};
