#ifndef CAN_H
#define CAN_H

#ifdef NATIVE
#include "../../test/mock/MockFlexCAN_T4.h"
#else
#include <FlexCAN_T4.h>
#endif

#include <Arduino.h>
#include <functional>
#include "../AnalogInput/AnalogInput.h"
#include "../dbc/tcs.h"

class Can {
  public:
    FlexCAN_T4<CAN1, RX_SIZE_16, TX_SIZE_16>& interface;
    AnalogInput& clutchRight;
    
    Can(
        FlexCAN_T4<CAN1, RX_SIZE_16, TX_SIZE_16>& interfaceRef,
        AnalogInput& clutchRightRef
    ) : 
        interface(interfaceRef),
        clutchRight(clutchRightRef) {
    }
    
    virtual void begin();
    virtual void broadcast(bool up, bool down, float clutchRight, float clutchLeft);
};

#endif
