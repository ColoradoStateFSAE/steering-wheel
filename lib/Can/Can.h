#ifndef CAN_H
#define CAN_H

#ifdef NATIVE
#include "../../test/mock/MockAdafruit_MCP2515.h"
#include "../../test/mock/MockAdafruit_NeoPixel.h"
#else
#include <Adafruit_MCP2515.h>
#include <Adafruit_NeoPixel.h>
#endif

#include <Arduino.h>
#include "../AnalogInput/AnalogInput.h"
#include <tcs.h>

class Can {
  public:
    Adafruit_MCP2515& mcp;
    AnalogInput& clutchRight;
    AnalogInput& clutchLeft;
    Adafruit_NeoPixel& pixels;
    
    Can(
        Adafruit_MCP2515& mcpRef,
        AnalogInput& clutchRightRef,
        AnalogInput& clutchLeftRef,
        Adafruit_NeoPixel& pixelsRef
    ) : 
        mcp(mcpRef),
        clutchRight(clutchRightRef),
        clutchLeft(clutchLeftRef),
        pixels(pixelsRef) {
            
    }
    
    virtual void begin();
    virtual void update();
    virtual void updateLed();
    virtual void broadcast(bool up, bool down, float clutchRight, float clutchLeft);
};

#endif
