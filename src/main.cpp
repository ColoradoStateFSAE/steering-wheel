#include <Arduino.h>
#include <optional>
#include <Adafruit_NeoPixel.h>
#include <OneButton.h>
#include <ClickEncoder.h>
#include "ClutchPaddle/ClutchPaddle.h"
#include "CanController/CanController.h"
#include "Rotary/Rotary.h"
#include "constants.h"

AsyncTimer timers;
Signals signals;

#ifdef PIO_UNIT_TESTING

using namespace fakeit;

// CanController
Mock<CanController> mockCanController;
CanController& canController = mockCanController.get();

// Up
Mock<OneButton> mockUp;
OneButton& up = mockUp.get();

// Down
Mock<OneButton> mockDown;
OneButton& down = mockDown.get();

// Clutch left
Mock<ClutchPaddle> mockClutchLeft;
ClutchPaddle& clutchLeft = mockClutchLeft.get();

// Clutch right
Mock<ClutchPaddle> mockClutchRight;
ClutchPaddle& clutchRight = mockClutchRight.get();

// NeoPixel
Mock<Adafruit_NeoPixel> mockPixels;
Adafruit_NeoPixel& pixels = mockPixels.get();

#else

std::optional<MCP2515> mcp;
Adafruit_NeoPixel pixels(3, 13, NEO_GRB + NEO_KHZ800);
CanController canController(*mcp, signals);
OneButton up;
OneButton down;
OneButton left;
OneButton right;
OneButton encoderButton;

ClutchPaddle clutchLeft;
ClutchPaddle clutchRight;

Rotary rotaryLeft(30, -2);
Rotary rotaryRight(0, 2);

#endif

void setup() {
    #ifdef ARDUINO_ARCH_RP2040
    mcp.emplace(MCP2515(CAN_CS));
    #endif

    canController.begin();

    pixels.begin();
    pixels.setBrightness(127);
    
    // ADC resolution
    analogReadResolution(12);

    // Clutch
    clutchLeft.begin(CLUTCH_LEFT, 26, 10, 10);
    clutchRight.begin(CLUTCH_RIGHT, 26, 10, 10);

    // Up
    up.setup(UP_BUTTON, INPUT_PULLUP, true);
    up.setDebounceMs(5);
    up.attachPress([]() {
        signals.onButton(UP);
    });

    // Down
    down.setup(DOWN_BUTTON, INPUT_PULLUP, true);
    down.setDebounceMs(5);
    down.attachPress([]() {
        signals.onButton(DOWN);
    });

    left.setup(25, INPUT_PULLUP, true);
    left.setDebounceMs(5);
    left.attachPress([]() {
        signals.onEncoder(false, -1);
    });

    right.setup(24, INPUT_PULLUP, true);
    right.setDebounceMs(5);
    right.attachPress([]() {
        signals.onEncoder(false, 1);
    });

    // Encoder button
    encoderButton.setup(21, INPUT_PULLUP, true);
    encoderButton.setDebounceMs(5);
    encoderButton.attachPress([]() {
        signals.onEncoder(true, 0);
    });

    timers.setInterval([&]() {
        uint32_t color = signals.offline ? 0xFF0000 : 0x00FF00;
        pixels.setPixelColor(0, color);
        pixels.show();
    }, 200);

    rotaryLeft.begin();
    rotaryRight.begin();
}

void loop() {
    up.tick();
    down.tick();
    left.tick();
    right.tick();
    encoderButton.tick();

    clutchLeft.update();
    clutchRight.update();

    signals.clutchLeft = clutchLeft.travel();
    signals.clutchRight = clutchRight.travel();

    signals.clutchLeftRaw = clutchLeft.readingRaw();
    signals.clutchRightRaw = clutchRight.readingRaw();

    canController.update();

    timers.handle();

    signals.rotaryLeftVolts = (rotaryLeft.position()) / 10.0;
    signals.rotaryRightVolts = (rotaryRight.position()) / 10.0;
}
