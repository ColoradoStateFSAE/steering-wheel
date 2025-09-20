#include <Arduino.h>
#include <optional>
#include <Adafruit_NeoPixel.h>
#include <OneButton.h>
#include "ClutchPaddle/ClutchPaddle.h"
#include "CanController/CanController.h"
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
Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
CanController canController(*mcp, signals);
OneButton up;
OneButton down;    
ClutchPaddle clutchLeft;
ClutchPaddle clutchRight;

#endif

void setup() {
    #ifdef ARDUINO_ARCH_RP2040
    mcp.emplace(MCP2515(PIN_CAN_CS));
    #endif

    canController.begin();

    // Clutch
    clutchLeft.begin(CLUTCH_LEFT, 26, 10, 10);
    clutchRight.begin(CLUTCH_RIGHT, 26, 10, 10);

    // Up
    up.setup(UP_BUTTON, INPUT_PULLUP);
    up.setDebounceMs(5);
    up.attachPress([]() {
        signals.onButton(UP);
    });

    // Down
    down.setup(DOWN_BUTTON, INPUT_PULLUP);
    down.setDebounceMs(5);
    down.attachPress([]() {
        signals.onButton(DOWN);
    });

    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, HIGH);
    pixels.begin();

    timers.setInterval([&]() {
        auto color = signals.offline ? RED : GREEN;
        pixels.setPixelColor(0, color);
        pixels.show();
    }, 200);
}

void loop() {
    up.tick();
    down.tick();

    clutchLeft.update();
    clutchRight.update();

    signals.clutchLeft = clutchLeft.travel();
    signals.clutchRight = 0;

    signals.clutchLeftRaw = clutchLeft.readingRaw();
    signals.clutchRightRaw = clutchRight.readingRaw();

    canController.update();

    timers.handle();
}
