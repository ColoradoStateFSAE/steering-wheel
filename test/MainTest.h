#if !defined(MAIN_TEST_H) && defined(NATIVE)
#define MAIN_TEST_H

#include <gtest/gtest.h>
#include <Arduino.h>
#include "mock/MockAdafruit_NeoPixel.h"
#include "../lib/AnalogInput/AnalogInput.h"
#include "../lib/Button/Button.h"
#include "../lib/Can/Can.h"

using namespace fakeit;

extern Mock<Adafruit_MCP2515> mockMcp;
extern Mock<Adafruit_NeoPixel> mockPixels;
extern Mock<Button> mockUp;
extern Mock<Button> mockDown;
extern Mock<AnalogInput> mockClutchRight;
extern Mock<AnalogInput> mockClutchLeft;
extern Mock<Can> mockCan;

extern void setup();
extern void loop();

class MainTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Adafruit_MCP2515
        When(Method(mockMcp, begin)).AlwaysReturn(1);

        // Adafruit_NeoPixel
        When(Method(mockPixels, begin)).AlwaysReturn();
        When(Method(mockPixels, setPixelColor)).AlwaysReturn();
        When(Method(mockPixels, show)).AlwaysReturn();

        // Up
        When(Method(mockUp, begin)).AlwaysReturn();
        When(Method(mockUp, update)).AlwaysReturn();
        When(Method(mockUp, pressed)).AlwaysReturn(false);

        // Down
        When(Method(mockDown, begin)).AlwaysReturn();
        When(Method(mockDown, update)).AlwaysReturn();
        When(Method(mockDown, pressed)).AlwaysReturn(false);

        // Right Clutch
        When(Method(mockClutchRight, begin)).AlwaysReturn();
        When(Method(mockClutchRight, minDeadzone)).AlwaysReturn();
        When(Method(mockClutchRight, maxDeadzone)).AlwaysReturn();
        When(Method(mockClutchRight, update)).AlwaysReturn();
        When(Method(mockClutchRight, travel)).AlwaysReturn(33.3);

        // Left Clutch
        When(Method(mockClutchLeft, begin)).AlwaysReturn();
        When(Method(mockClutchLeft, minDeadzone)).AlwaysReturn();
        When(Method(mockClutchLeft, maxDeadzone)).AlwaysReturn();
        When(Method(mockClutchLeft, update)).AlwaysReturn();
        When(Method(mockClutchLeft, travel)).AlwaysReturn(0);

        // Can
        When(Method(mockCan, begin)).AlwaysReturn();
        When(Method(mockCan, update)).AlwaysReturn();
        When(Method(mockCan, updateLed)).AlwaysReturn();
        When(Method(mockCan, broadcast)).AlwaysReturn();

        // Arduino core mocks
        When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
        When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
        When(Method(ArduinoFake(), millis)).AlwaysReturn(0);

        setup();
    }

    void TearDown() override {
        ArduinoFakeReset();
        mockCan.ClearInvocationHistory();
        mockUp.ClearInvocationHistory();
        mockDown.ClearInvocationHistory();
        mockClutchRight.ClearInvocationHistory();
        mockClutchLeft.ClearInvocationHistory();
    }
};

TEST_F(MainTest, setup) {
    Verify(
        Method(mockMcp, begin).Using(1000000),

        Method(ArduinoFake(), pinMode).Using(20, OUTPUT),
        Method(ArduinoFake(), digitalWrite).Using(20, HIGH),
        
        Method(mockClutchRight, begin).Using(17),
        Method(mockClutchRight, minDeadzone).Using(10),
        Method(mockClutchRight, maxDeadzone).Using(20),

        Method(mockClutchLeft, begin).Using(16),
        Method(mockClutchLeft, minDeadzone).Using(10),
        Method(mockClutchLeft, maxDeadzone).Using(20),

        Method(mockUp, begin).Using(4, 5000),
        Method(mockDown, begin).Using(2, 5000)
    );
}

TEST_F(MainTest, loop) {
    loop();
    Verify(
        Method(mockCan, update),
        Method(mockCan, updateLed),

        Method(mockUp, update),
        Method(mockDown, update),

        Method(mockClutchRight, update),
        Method(mockClutchLeft, update)
    );
}

TEST_F(MainTest, ButtonsPressed) {
    When(Method(mockUp, pressed)).AlwaysReturn(true);
    loop();
    Verify(Method(mockCan, broadcast).Using(true, false, 33.3f, 0)).Once();

    When(Method(mockUp, pressed)).AlwaysReturn(false);
    When(Method(mockDown, pressed)).AlwaysReturn(true);
    loop();
    Verify(Method(mockCan, broadcast).Using(false, true, 33.3f, 0)).Once();
}

TEST_F(MainTest, TimedBroadcast) {
    for(int i=0; i<=1000; i++) {
        When(Method(ArduinoFake(), millis)).AlwaysReturn(i);
        loop();
    }

    Verify(Method(mockCan, broadcast)).Exactly(100);
}

#endif
