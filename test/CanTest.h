#if !defined(CAN_TEST_H) && defined(NATIVE)
#define CAN_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Arduino.h>
#include "../lib/Can/Can.h"
#include "mock/MockAdafruit_MCP2515.h"
#include <vector>

using namespace fakeit;

class CanTest : public ::testing::Test {
  protected:
    Mock<Adafruit_MCP2515> mockMcp;
    Adafruit_MCP2515& mcp = mockMcp.get();

    Mock<AnalogInput> mockClutchRight;
    AnalogInput& clutchRight = mockClutchRight.get();

    Mock<AnalogInput> mockClutchLeft;
    AnalogInput& clutchLeft = mockClutchLeft.get();

    Mock<Adafruit_NeoPixel> mockPixels;
    Adafruit_NeoPixel& pixels = mockPixels.get();

    Can can = Can(mcp, clutchRight, clutchLeft, pixels);

    unsigned long time = 0;

    void SetUp() override {
        When(Method(mockMcp, beginPacket)).AlwaysReturn(1);
        When(Method(mockMcp, write)).AlwaysReturn(1);
        When(Method(mockMcp, endPacket)).AlwaysReturn(1);
        When(Method(mockMcp, parsePacket)).AlwaysReturn(8);

        When(Method(mockPixels, show)).AlwaysReturn();
        When(Method(mockPixels, setPixelColor)).AlwaysReturn();

        When(Method(mockClutchRight, travel)).AlwaysReturn(0);
        When(Method(mockClutchLeft, travel)).AlwaysReturn(0);
        When(Method(ArduinoFake(), analogRead)).AlwaysReturn(0);

        When(Method(ArduinoFake(), millis)).AlwaysDo([&]() -> unsigned long { return time; });

        can.begin();
    }

    void TearDown() override {
        ArduinoFakeReset();
    }
};

TEST_F(CanTest, broadcastUp) {
    const auto expectedID = 0x65C;
    const uint8_t expected[8] = {0b10000000, 0x17, 0x70, 0x0F, 0xA0, 0x00, 0x00, 0x00};

    size_t actualSize = 0;
    uint8_t actual[8] = {0};

    When(Method(mockMcp, write)).Do([&](uint8_t *buffer, size_t size) -> int {
        actualSize = size;
        memcpy(actual, buffer, sizeof(actual));
        return 1;
    });

    can.broadcast(true, false, 60, 40);

    Verify(Method(mockMcp, beginPacket).Using(expectedID)).Once();
    Verify(Method(mockMcp, endPacket)).Once();

    EXPECT_EQ(actualSize, 8);
    EXPECT_TRUE(0 == memcmp(actual, expected, sizeof(actual)));
}

TEST_F(CanTest, broadcastDown) {
    const auto expectedID = 0x65C;
    const uint8_t expected[8] = {0b01000000, 0x17, 0x70, 0x0F, 0xA0, 0x00, 0x00, 0x00};

    size_t actualSize = 0;
    uint8_t actual[8] = {0};

    When(Method(mockMcp, write)).Do([&](uint8_t *buffer, size_t size) -> int {
        actualSize = size;
        memcpy(actual, buffer, sizeof(actual));
        return 1;
    });

    can.broadcast(false, true, 60, 40);

    Verify(Method(mockMcp, beginPacket).Using(expectedID)).Once();
    Verify(Method(mockMcp, endPacket)).Once();

    EXPECT_EQ(actualSize, 8);
    for(size_t i=0; i<actualSize; i++) {
        EXPECT_EQ(actual[i], expected[i]) << "Mismatch at index " << i;
    }
}

TEST_F(CanTest, broadcastMin) {
    const auto expectedID = 0x65cu;
    const uint8_t expected[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    size_t actualSize = 0;
    uint8_t actual[8] = {0};

    When(Method(mockMcp, write)).Do([&](uint8_t *buffer, size_t size) -> int {
        actualSize = size;
        memcpy(actual, buffer, sizeof(actual));
        return 1;
    });

    can.broadcast(false, false, -1, -1);

    Verify(Method(mockMcp, beginPacket).Using(expectedID)).Once();
    Verify(Method(mockMcp, endPacket)).Once();

    EXPECT_EQ(actualSize, 8);
    for(size_t i=0; i<actualSize; i++) {
        EXPECT_EQ(actual[i], expected[i]) << "Mismatch at index " << i;
    }
}

TEST_F(CanTest, broadcastMax) {
    const auto expectedID = 0x65cu;
    const uint8_t expected[8] = {0x00, 0x27, 0x10, 0x27, 0x10, 0x00, 0x00, 0x00};

    size_t actualSize = 0;
    uint8_t actual[8] = {0};

    When(Method(mockMcp, write)).Do([&](uint8_t *buffer, size_t size) -> int {
        actualSize = size;
        memcpy(actual, buffer, sizeof(actual));
        return 1;
    });

    can.broadcast(false, false, 101, 101);

    Verify(Method(mockMcp, beginPacket).Using(expectedID)).Once();
    Verify(Method(mockMcp, endPacket)).Once();

    EXPECT_EQ(actualSize, 8);
    for(size_t i=0; i<actualSize; i++) {
        EXPECT_EQ(actual[i], expected[i]) << "Mismatch at index " << i;
    }
}

TEST_F(CanTest, ledColor) {
    can.update();
    Verify(Method(mockPixels, setPixelColor).Using(0, pixels.Color(0, 255, 0)));

    When(Method(mockMcp, parsePacket)).AlwaysReturn(0);

    time = 100;
    can.update();
    Verify(Method(mockPixels, setPixelColor).Using(0, pixels.Color(255, 0, 0)));
}

TEST_F(CanTest, updateLedFrequency) {
    for(int i=0; i<=2000; i+=10) {
        time = i;
        can.updateLed();
    }

    Verify(Method(mockPixels, show)).Exactly(4);
}

#endif
