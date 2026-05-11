#pragma once
#include <Arduino.h>

class Rotary {
    int _basePin;
    int _offset = 0;
    int _lastPosition;
public:
    Rotary(int basePin, int offset=0) : _basePin(basePin), _offset(offset){}
    void begin();
    int position();
};
