#pragma once
#ifdef PIO_UNIT_TESTING

#include <gtest/gtest.h>
#include <Arduino.h>
#include <optional>
#include <constants.h>
#include "ClutchPaddle/ClutchPaddle.h"

using namespace std;
using namespace fakeit;

class ClutchPaddleTest : public ::testing::Test {
  protected:
    int reading = 500;
    ClutchPaddle clutchPaddle;

    void SetUp() override {
        When(Method(ArduinoFake(), analogRead).Using(10)).AlwaysDo([&](uint8_t) -> int { return reading; });

        clutchPaddle.begin(10, 0, 0, 0);
    }

    void TearDown() override {
        ArduinoFakeReset();
    }
};

TEST_F(ClutchPaddleTest, calibration) {
    When(Method(ArduinoFake(), analogRead)).Return(500, 550, 600, 500, 400, 500);
    std::vector<int> ASSERTed = {0, 0, 100, 0, 0, 50};

    for(auto i : ASSERTed) {
        clutchPaddle.update();
        ASSERT_EQ(clutchPaddle.travel(), i);
    }
}

// TEST_F(ClutchPaddleTest, update) {
//     clutchPaddle.update();
//     // Verify(Method(mockAnalogPin, readSmoothed)).Once();
// }

// TEST_F(ClutchPaddleTest, defaultDeadzones) {
//     // Test that the default deadzones work
//     resample(400);
//     resample(600);

//     // Test min deadzone
//     std::vector<int> min = {400, 390, 380, 370, 360};
//     for(auto reading : min) {
//         resample(reading);
//         ASSERT_EQ(clutchPaddle.travel(), 0);
//     }

//     // Test max deadzone
//     std::vector<int> max = {600, 610, 620, 630, 640};
//     for(auto reading : max) {
//         resample(reading);
//         ASSERT_EQ(clutchPaddle.travel(), 100);
//     }
// }


// TEST_F(ClutchPaddleTest, deadzones) {
//     // Test that custom deadzones work
//     clutchPaddle.minDeadzone(20);
//     clutchPaddle.maxDeadzone(20);

//     resample(400);
//     resample(600);

//     // Test min deadzone
//     std::vector<int> min = {400, 410, 420, 430, 440};
//     for(auto reading : min) {
//         resample(reading);
//         ASSERT_EQ(clutchPaddle.travel(), 0);
//     }

//     // Test max deadzone
//     std::vector<int> max = {600, 590, 580, 570, 560};
//     for(auto reading : max) {
//         resample(reading);
//         ASSERT_EQ(clutchPaddle.travel(), 100);
//     }
// }

#endif
