#ifndef MAIN_CPP
#define MAIN_CPP

#include <Arduino.h>
#include "../lib/AnalogInput/AnalogInput.h"
#include "../lib/Button/Button.h"
#include "../lib/Can/Can.h"

#ifdef NATIVE
    #include "../test/mock/MockFlexCAN_T4.h"

    using namespace fakeit;

    Mock<Can> mockCan;
    Mock<Button> mockUp;
    Mock<Button> mockDown;
    Mock<AnalogInput> mockClutchRight;

    Can& can = mockCan.get();
    Button& up = mockUp.get();
    Button& down = mockDown.get();
    AnalogInput& clutchRight = mockClutchRight.get();
#else
    #include <FlexCAN_T4.h>

    FlexCAN_T4<CAN1, RX_SIZE_16, TX_SIZE_16> interface;
    AnalogInput clutchRight(512);
    Can can(interface, clutchRight);
    Button up;
    Button down;
#endif

const int UP = 6;
const int DOWN = 7;
const int CLUTCH_LEFT = 16;
const int CLUTCH_RIGHT = 17;

void setup() {
    can.begin();

    up.begin(UP);
    down.begin(DOWN);

    clutchRight.begin(CLUTCH_RIGHT);
    clutchRight.minDeadzone(10);
    clutchRight.maxDeadzone(20);
}

void loop() {
    // Handle input
    up.update();
    down.update();
    clutchRight.update();

    if(up.pressed()) {
        can.broadcast(true, false, clutchRight.travel(), 0);
    } else if(down.pressed()) {
        can.broadcast(false, true, clutchRight.travel(), 0);
    } else {
        // Broadcast clutch every 10 ms
        static unsigned long lastBroadastTime = 0;
        if(millis() - lastBroadastTime < 10) return;

        can.broadcast(false, false, clutchRight.travel(), 0);
        lastBroadastTime = millis();
    }
}

#endif
