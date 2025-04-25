#include "AnalogInput.h"

using namespace std;

void AnalogInput::begin(int pin) {
    _pin = pin;    
    pinMode(_pin, INPUT_PULLUP);

    while(!runningAverage.bufferIsFull()) {
        runningAverage.add(analogRead(_pin));
    }

    findMinMax(runningAverage.getFastAverage());
}

void AnalogInput::minDeadzone(float percent) {
    _minDeadzone = percent / 100.0f;
}

void AnalogInput::maxDeadzone(float percent) {
    _maxDeadzone = percent / 100.0f;
}

void AnalogInput::findMinMax(int value) {
    // Update the maximum if a larger value is found
    if(max < value) {
        max = value;
    }

    if(value < min) {
        min = value;
    }
}

void AnalogInput::update() {
    const int reading = analogRead(_pin);
    
    runningAverage.add(reading);
    float average = runningAverage.getFastAverage();
    findMinMax(average);
}

float AnalogInput::travel() {
    if(max == 0 || min == std::numeric_limits<float>::max()) return 0;
    if(abs(max - min) < 50) return 0;

    float average = runningAverage.getFastAverage();

    float range = abs(max - min);
    float minDeadzoneScaled = min + range * _minDeadzone;
    float maxDeadzoneScaled = max - range * _maxDeadzone;

    float percentage = map(average, minDeadzoneScaled, maxDeadzoneScaled, 0.0f, 100.0f);
    percentage = constrain(percentage, 0.0f, 100.0f);

    return percentage;
}
