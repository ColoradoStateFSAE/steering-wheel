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
    std::vector<int> expected = {0, 0, 100, 0, 0, 50};

    for(auto i : expected) {
        clutchPaddle.update();
        ASSERT_EQ(clutchPaddle.travel(), i);
    }
}

TEST_F(ClutchPaddleTest, readingRaw) {
    When(Method(ArduinoFake(), analogRead)).Return(500, 510, 520, 530, 540, 550);
    std::vector<int> expected = {500, 510, 520, 530, 540, 550};

    for(auto i : expected) {
        ASSERT_EQ(clutchPaddle.readingRaw(), i);
    }
}

#endif
