#include "ClutchPaddle.h"

using namespace std;

void ClutchPaddle::begin(uint8_t _pin, uint8_t _alpha, float _minDeadzone, float _maxDeadzone) {
    pin = _pin;
    analogPin.emplace(_pin);
    analogPin->setSmoothWeight(_alpha);

    limitMin = 0 - _minDeadzone;
    limitMax = 100 + _maxDeadzone;

    reading = analogPin->readSmoothed();
}

void ClutchPaddle::update() {
    reading = analogPin->readSmoothed();
    readingMin = min(readingMin, reading);
    readingMax = max(readingMax, reading);
    updated = true;
}

float ClutchPaddle::travel() {
    int range = abs(readingMax - readingMin);
    if(range < 100) return 0;

    float percentage = map<float>(reading, readingMin, readingMax, limitMin, limitMax);

    return constrain(percentage, 0, 100);
}

int ClutchPaddle::readingRaw() {
    return analogRead(pin);
}
