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
OneButton encoderButton;

ClutchPaddle clutchLeft;
ClutchPaddle clutchRight;

Rotary rotaryLeft(30);
Rotary rotaryRight(0);

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

    // Encoder button
    encoderButton.setup(21, INPUT_PULLUP, true);
    encoderButton.setDebounceMs(5);
    encoderButton.attachPress([]() {
        Serial.println("Encoder");
    });

    timers.setInterval([&]() {
        uint32_t color = signals.offline ? 0xFF0000 : 0x00FF00;
        pixels.setPixelColor(0, color);
        pixels.show();
    }, 200);

    rotaryLeft.begin();
    rotaryRight.begin();

    pinMode(22, INPUT_PULLUP);
    pinMode(23, INPUT_PULLUP);
}

ClickEncoder encoder(22, 23, 21, 1, LOW);

float lastLeft = rotaryLeft.position();
float lastRight = rotaryRight.position();

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

    encoder.service();

    int16_t delta = encoder.getIncrement();
    if (delta != 0) {
        Serial.print("Increment: ");
        Serial.print(delta);
        Serial.print("  |  Total: ");
        Serial.println(encoder.getAccumulate());
    }
}
