#pragma once
#ifdef PIO_UNIT_TESTING
#include <gtest/gtest.h>
#include <Arduino.h>
#include <OneButton.h>
#include <Adafruit_NeoPixel.h>
#include "ClutchPaddle/ClutchPaddle.h"
#include "CanController/CanController.h"
#include "../src/constants.h"

using namespace std;
using namespace fakeit;

extern Signals signals;
extern Mock<OneButton> mockUp;
extern Mock<OneButton> mockDown;
extern Mock<ClutchPaddle> mockClutchLeft;
extern Mock<ClutchPaddle> mockClutchRight;
extern Mock<Adafruit_NeoPixel> mockPixels;
extern Mock<CanController> mockCanController;

extern void setup();
extern void loop();

class MainTest : public ::testing::Test {
protected:
    callbackFunction upCallback = nullptr;
    callbackFunction downCallback = nullptr;

    unsigned long time = 0;

    void SetUp() override {
        ArduinoFakeReset();
        mockCanController.ClearInvocationHistory();
        mockPixels.ClearInvocationHistory();
        mockUp.ClearInvocationHistory();
        mockDown.ClearInvocationHistory();
        mockClutchLeft.ClearInvocationHistory();
        mockClutchRight.ClearInvocationHistory();

        When(Method(ArduinoFake(), millis)).AlwaysDo([&]() -> unsigned long { return time; });

        // CanController
        When(Method(mockCanController, begin)).AlwaysReturn();
        When(Method(mockCanController, update)).AlwaysReturn();
        
        // NeoPixels
        When(Method(mockPixels, begin)).AlwaysReturn();
        When(Method(mockPixels, setPixelColor)).AlwaysReturn();
        When(Method(mockPixels, show)).AlwaysReturn();

        // Up and down
        for(auto button : {ref(mockUp), ref(mockDown)}) {
            When(Method(button.get(), setup)).AlwaysReturn();
            When(Method(button.get(), setDebounceMs)).AlwaysReturn();
            When(Method(button.get(), tick)).AlwaysReturn();
        }

        When(Method(mockUp, attachPress)).AlwaysDo([&](callbackFunction cb) { upCallback = cb; });
        When(Method(mockDown, attachPress)).AlwaysDo([&](callbackFunction cb) { downCallback = cb; });

        // ClutchLeft and clutchRight
        for(auto clutch : {ref(mockClutchLeft), ref(mockClutchRight)}) {
            When(Method(clutch.get(), begin)).AlwaysReturn();
            When(Method(clutch.get(), update)).AlwaysReturn();
            When(Method(clutch.get(), travel)).AlwaysReturn(55);
            When(Method(clutch.get(), readingRaw)).AlwaysReturn(300);
        }

        // NeoPixels
        When(Method(mockPixels, begin)).AlwaysReturn();
        When(Method(mockPixels, show)).AlwaysReturn();

        When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
        When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
    }
};

TEST_F(MainTest, setup) {
    setup();

    Verify(
        Method(mockCanController, begin),
        Method(mockClutchLeft, begin).Using(CLUTCH_LEFT, 26, 10, 10),
        Method(mockClutchRight, begin).Using(CLUTCH_RIGHT, 26, 10, 10),
        Method(mockUp, setup).Using(UP_BUTTON, INPUT_PULLUP, false),
        Method(mockUp, setDebounceMs).Using(5),
        Method(mockUp, attachPress),
        Method(mockDown, setup).Using(DOWN_BUTTON, INPUT_PULLUP, false),
        Method(mockDown, setDebounceMs).Using(5),
        Method(mockDown, attachPress),
        Method(ArduinoFake(), pinMode).Using(20, OUTPUT),
        Method(ArduinoFake(), digitalWrite).Using(20, HIGH),
        Method(mockPixels, begin)
    );

    bool upCalled = false;
    bool downCalled = false;
    signals.onButton = [&](ShiftDirection direction) {
        upCalled |= direction == UP;
        downCalled |= direction == DOWN;
    };

    upCallback();
    downCallback();

    ASSERT_TRUE(upCalled);
    ASSERT_TRUE(downCalled);
}

TEST_F(MainTest, loop) {
    When(Method(mockClutchRight, travel)).AlwaysReturn(30);
    When(Method(mockClutchRight, readingRaw)).AlwaysReturn(500);

    loop();
    
    Verify(
        Method(mockUp, tick),
        Method(mockDown, tick),
        Method(mockCanController, update)
    );

    ASSERT_EQ(signals.clutchLeft, 55);
    ASSERT_EQ(signals.clutchRight, 0);

    ASSERT_EQ(signals.clutchLeftRaw, 300);
    ASSERT_EQ(signals.clutchRightRaw, 500);

    signals.offline = false;
}

TEST_F(MainTest, pixels) {
    for(time = 0; time <= 200 * 5; time += 200) {
        loop();
    }

    Verify(Method(mockPixels, setPixelColor).Using(0, GREEN)).Exactly(5);
    Verify(Method(mockPixels, show)).Exactly(5);

    signals.offline = true;
    time += 200;
    loop();
    Verify(Method(mockPixels, setPixelColor).Using(0, RED)).Exactly(1);
}

#endif
