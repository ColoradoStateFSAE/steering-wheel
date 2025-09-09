#pragma once

#include <Arduino.h>
#include <mcp2515.h>
#include <AsyncTimer.h>
#include <rrcan.h>
#include "Signals/Signals.h"
#include "constants.h"
#include "macros.h"

class CanController {
  public:
    CanController(MCP2515& _mcp, Signals& _signals) : mcp(_mcp), signals(_signals) { }
    virtual void begin();
    virtual void update();

  private:
    MCP2515& mcp;
    Signals& signals;
    AsyncTimer timers;

    unsigned long lastUpdateTime = 0;

    void SEND_MESSAGE_H(swc_clutch);
    void sendShift(ShiftDirection direction);
};
