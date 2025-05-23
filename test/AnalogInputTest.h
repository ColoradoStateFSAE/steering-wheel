#if !defined(ANALOG_INPUT_TEST_H) && defined(NATIVE)
#define ANALOG_INPUT_TEST_H

#include <gtest/gtest.h>
#include <Arduino.h>
#include <vector>
#include "../lib/AnalogInput/AnalogInput.h"

using namespace fakeit;

class AnalogInputTest : public ::testing::Test {
  protected:
    const int samples = 1000;
    const int pin = 17;
    AnalogInput analogInput = AnalogInput(samples);

    int analogValue = 500;

    void resample(int value, int n=1000) {
        analogValue = value;
        for (int i=0; i<n; i++) {
            analogInput.update();
        }
    }

    void SetUp() override {
        When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
        When(Method(ArduinoFake(), analogRead)).AlwaysDo([&](uint8_t) -> int { return analogValue; });
        When(Method(ArduinoFake(), delay)).AlwaysReturn();
        When(Method(ArduinoFake(), millis)).AlwaysReturn(0);
        When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const String&))).AlwaysReturn(0);

        When(Method(ArduinoFake(), map)).AlwaysDo([](long x, long in_min, long in_max, long out_min, long out_max) {
            return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        });

        analogInput.begin(pin);
    }

    void TearDown() override {
        ArduinoFakeReset();
    }
};

TEST_F(AnalogInputTest, begin) {
    Verify(Method(ArduinoFake(), pinMode).Using(pin, INPUT_PULLUP)).Once();
    Verify(Method(ArduinoFake(), analogRead).Using(pin)).Exactly(samples);
    EXPECT_EQ(analogInput.travel(), 0);
}

TEST_F(AnalogInputTest, update) {
    analogInput.update();
    Verify(Method(ArduinoFake(), analogRead).Using(pin)).Exactly(samples + 1);
}

TEST_F(AnalogInputTest, defaultDeadzones) {
    // Test that the default deadzones work
    resample(400);
    resample(600);

    // Test min deadzone
    std::vector<int> min = {400, 390, 380, 370, 360};
    for(auto reading : min) {
        resample(reading);
        EXPECT_EQ(analogInput.travel(), 0);
    }

    // Test max deadzone
    std::vector<int> max = {600, 610, 620, 630, 640};
    for(auto reading : max) {
        resample(reading);
        EXPECT_EQ(analogInput.travel(), 100);
    }
}


TEST_F(AnalogInputTest, deadzones) {
    // Test that custom deadzones work
    analogInput.minDeadzone(20);
    analogInput.maxDeadzone(20);

    resample(400);
    resample(600);

    // Test min deadzone
    std::vector<int> min = {400, 410, 420, 430, 440};
    for(auto reading : min) {
        resample(reading);
        EXPECT_EQ(analogInput.travel(), 0);
    }

    // Test max deadzone
    std::vector<int> max = {600, 590, 580, 570, 560};
    for(auto reading : max) {
        resample(reading);
        EXPECT_EQ(analogInput.travel(), 100);
    }
}

#endif
