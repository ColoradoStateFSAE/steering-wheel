#pragma once
#include <Arduino.h>

class Rotary {
    int _basePin;
    int _lastPosition = -1;
public:
    Rotary(int basePin) : _basePin(basePin) {}
    void begin();
    int position();
};
