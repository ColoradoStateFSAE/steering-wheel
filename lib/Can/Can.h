#ifndef CAN_H
#define CAN_H

#ifdef NATIVE
#include "../../test/mock/MockAdafruit_MCP2515.h"
#else
#include <Adafruit_MCP2515.h>
#endif

#include <Arduino.h>
#include <functional>
#include "../AnalogInput/AnalogInput.h"
#include <tcs.h>

class Can {
  public:
    Adafruit_MCP2515& mcp;
    AnalogInput& clutchRight;
    AnalogInput& clutchLeft;
    
    Can(
        Adafruit_MCP2515& mcpRef,
        AnalogInput& clutchRightRef,
        AnalogInput& clutchLeftRef
    ) : 
        mcp(mcpRef),
        clutchRight(clutchRightRef),
        clutchLeft(clutchLeftRef) {
            
    }
    
    virtual void begin();
    virtual void broadcast(bool up, bool down, float clutchRight, float clutchLeft);
};

#endif
