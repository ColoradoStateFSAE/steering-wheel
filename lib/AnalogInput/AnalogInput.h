#ifndef ANALOG_INPUT_H
#define ANALOG_INPUT_H

#include <Arduino.h>
#include <RunningAverage.h>
#include <limits>

class AnalogInput {
  public:
    enum State {
        READING,
        IDLE,
        RECALIBRATE,
    };

    AnalogInput() = delete;
    AnalogInput(uint16_t samples) : _samples(samples) {};
    virtual ~AnalogInput() {}
    virtual void begin(int pin);
    virtual void minDeadzone(float percent);
    virtual void maxDeadzone(float percent);
    virtual void update();
    virtual float travel();
    
  private:
    int _pin = 0;
    uint16_t _samples;
    RunningAverage runningAverage = RunningAverage(_samples);

    float _minDeadzone = 0;
    float _maxDeadzone = 0;

    float min = std::numeric_limits<float>::max();
    float max = 0;

    void findMinMax(int value);
};

#endif
