#pragma once

#include <Arduino.h>
#include <AnalogPin.h>
#include <optional>
#include <utils.h>

class ClutchPaddle {
  public:
    virtual void begin(uint8_t _pin,  uint8_t _alpha, float _minDeadzone=0, float _maxDeadzone=0);
    virtual void update();
    virtual float travel();
    virtual int readingRaw();
    
  private:
    uint8_t pin;
    std::optional<AnalogPin> analogPin;
    int maxDifference = 0;

    int limitMin;
    int limitMax;
    bool updated = false;

    int reading = 0;
    int readingMin = 1023;
    int readingMax = 0;
};
