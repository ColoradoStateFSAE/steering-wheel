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
    #include "../test/mock/MockAdafruit_NeoPixel.h"

    using namespace fakeit;

    Mock<Adafruit_MCP2515> mockMcp;
    Mock<Adafruit_NeoPixel> mockPixels;
    Mock<AnalogInput> mockClutchRight;
    Mock<AnalogInput> mockClutchLeft;
    Mock<Button> mockUp;
    Mock<Button> mockDown;
    Mock<Can> mockCan;

    Adafruit_MCP2515& mcp = mockMcp.get();
    Adafruit_NeoPixel& pixels = mockPixels.get();
    AnalogInput& clutchRight = mockClutchRight.get();
    AnalogInput& clutchLeft = mockClutchLeft.get();
    Button& up = mockUp.get();
    Button& down = mockDown.get();
    Can& can = mockCan.get();
    
#else
    #include <Adafruit_NeoPixel.h>

    Adafruit_MCP2515 mcp(PIN_CAN_CS);
    Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
    AnalogInput clutchRight(256);
    AnalogInput clutchLeft(256);
    Button up;
    Button down;
    Can can(mcp, clutchRight, clutchLeft, pixels);
#endif

void setup() {
    mcp.begin(1000000);
    pinMode(20,OUTPUT);
    digitalWrite(20, HIGH);
    pixels.begin();

    clutchRight.begin(CLUTCH_RIGHT);
    clutchRight.minDeadzone(10);
    clutchRight.maxDeadzone(20);

    clutchLeft.begin(CLUTCH_LEFT);
    clutchLeft.minDeadzone(10);
    clutchLeft.maxDeadzone(20);

    up.begin(UP);
    down.begin(DOWN);
}

void loop() {
    can.update();
    can.updateLed();
    
    // Handle input
    up.update();
    down.update();
    clutchRight.update();
    clutchLeft.update();

    if(up.pressed()) {
        can.broadcast(true, false, clutchRight.travel(), clutchLeft.travel());
    } else if(down.pressed()) {
        can.broadcast(false, true, clutchRight.travel(), clutchLeft.travel());
    } else {
        // Broadcast clutch every 10 ms
        static unsigned long lastBroadastTime = 0;
        if(millis() - lastBroadastTime >= 10) {
            can.broadcast(false, false, clutchRight.travel(), clutchLeft.travel());
            lastBroadastTime = millis();
        }
    }
}

#endif
