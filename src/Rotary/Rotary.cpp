#include "Rotary.h"

void Rotary::begin() {
    for (int i = 0; i < 10; i++) {
        pinMode(_basePin + i, INPUT_PULLUP);
    }
}

int Rotary::position() {
    for (int i = 0; i < 10; i++) {
        if (digitalRead(_basePin + i) == LOW) {
            _lastPosition = i + 1;
            break;
        }
    }

    return _lastPosition;
}
