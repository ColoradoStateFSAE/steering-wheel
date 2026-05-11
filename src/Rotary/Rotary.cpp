#include "Rotary.h"

void Rotary::begin() {
    for (int i = 0; i < 10; i++) {
        pinMode(_basePin + i, INPUT_PULLUP);
    }
}

int Rotary::position() {
    _lastPosition = -1;

    for (int i = 0; i < 10; i++) {
        if (digitalRead(_basePin + i) == LOW) {
            _lastPosition = i;
            break;
        }
    }

    return (_lastPosition + _offset + 10) % 10;
}
