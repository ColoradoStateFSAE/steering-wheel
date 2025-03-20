#ifndef MAIN_CPP
#define MAIN_CPP

#include <Arduino.h>
#include "../lib/AnalogInput/AnalogInput.h"
#include "../lib/Button/Button.h"
#include "../lib/Can/Can.h"

const int UP = 6;
const int DOWN = 7;
const int CLUTCH_RIGHT = 17;
const int CLUTCH_LEFT = 16;

#ifdef NATIVE
    #include "../test/mock/MockAdafruit_MCP2515.h"

    using namespace fakeit;

    Mock<Can> mockCan;
    Mock<Button> mockUp;
    Mock<Button> mockDown;
    Mock<AnalogInput> mockClutchRight;
    Mock<AnalogInput> mockClutchLeft;

    Can& can = mockCan.get();
    Button& up = mockUp.get();
    Button& down = mockDown.get();
    AnalogInput& clutchRight = mockClutchRight.get();
    AnalogInput& clutchLeft = mockClutchLeft.get();
#else
    #include <Adafruit_MCP2515.h>

    Adafruit_MCP2515 mcp(PIN_CAN_CS);
    AnalogInput clutchRight(256);
    AnalogInput clutchLeft(256);
    Can can(mcp, clutchRight, clutchLeft);
    Button up;
    Button down;
#endif

void setup() {
    can.begin();

    up.begin(UP);
    down.begin(DOWN);

    clutchRight.begin(CLUTCH_RIGHT);
    clutchRight.minDeadzone(10);
    clutchRight.maxDeadzone(20);

    clutchLeft.begin(CLUTCH_LEFT);
    clutchLeft.minDeadzone(10);
    clutchLeft.maxDeadzone(20);
}

void loop() {
    // Handle input
    up.update();
    down.update();
    clutchRight.update();

    if(up.pressed()) {
        can.broadcast(true, false, clutchRight.travel(), clutchLeft.travel());
    } else if(down.pressed()) {
        can.broadcast(false, true, clutchRight.travel(), clutchLeft.travel());
    } else {
        // Broadcast clutch every 10 ms
        static unsigned long lastBroadastTime = 0;
        if(millis() - lastBroadastTime < 10) return;

        can.broadcast(false, false, clutchRight.travel(), clutchLeft.travel());
        lastBroadastTime = millis();
    }
}

#endif
