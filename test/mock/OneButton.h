#pragma once
#ifdef PIO_UNIT_TESTING

#include <Arduino.h>

extern "C" {
  typedef void (*callbackFunction)(void);
}

class OneButton {
  public:
    OneButton() = default;
    virtual ~OneButton() = default;
    virtual int setup(const uint8_t pin, const uint8_t mode, const bool activeLow=false) = 0;
    virtual int setDebounceMs(int ms) = 0;
    virtual void attachPress(callbackFunction newFunction) = 0;
    virtual void tick() = 0;
};

#endif
