#if !defined(MAIN_TEST_H) && defined(NATIVE)
#define MAIN_TEST_H

#include <gtest/gtest.h>
#include <Arduino.h>
#include "../lib/AnalogInput/AnalogInput.h"
#include "../lib/Button/Button.h"
#include "../lib/Can/Can.h"

using namespace fakeit;

extern Mock<Can> mockCan;
extern Mock<Button> mockUp;
extern Mock<Button> mockDown;
extern Mock<AnalogInput> mockClutchRight;

extern void setup();
extern void loop();

class MainTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Can
        When(Method(mockCan, begin)).AlwaysReturn();
        When(Method(mockCan, broadcast)).AlwaysReturn();

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

        When(Method(ArduinoFake(), millis)).AlwaysReturn(0);

        setup();
    }

    void TearDown() override {
        ArduinoFakeReset();
        mockCan.ClearInvocationHistory();
        mockUp.ClearInvocationHistory();
        mockDown.ClearInvocationHistory();
        mockClutchRight.ClearInvocationHistory();
    }
};

TEST_F(MainTest, CanBegin) {
    Verify(Method(mockCan, begin)).Once();
}

TEST_F(MainTest, ButtonBegin) {
    Verify(Method(mockUp, begin).Using(6, 5000)).Once();
    Verify(Method(mockDown, begin).Using(7, 5000)).Once();
}

TEST_F(MainTest, ClutchRightBegin) {
    Verify(Method(mockClutchRight, begin).Using(17)).Once();
    Verify(Method(mockClutchRight, minDeadzone).Using(10)).Once();
    Verify(Method(mockClutchRight, maxDeadzone).Using(20)).Once();
}

TEST_F(MainTest, ButtonsUpdate) {
    loop();
    Verify(Method(mockUp, update)).Once();
    Verify(Method(mockDown, update)).Once();
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
    for(int i=0; i<=1000; i+=5) {
        When(Method(ArduinoFake(), millis)).AlwaysReturn(i);
        loop();
    }

    Verify(Method(mockCan, broadcast)).Exactly(100);
}

#endif
