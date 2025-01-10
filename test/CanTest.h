#if !defined(CAN_TEST_H) && defined(NATIVE)
#define CAN_TEST_H

#include <gtest/gtest.h>
#include <Arduino.h>
#include "../lib/Can/Can.h"
#include "mock/MockFlexCAN_T4.h"

using namespace fakeit;

class CanTest : public ::testing::Test {
  protected:
    Mock<FlexCAN_T4<CAN1, RX_SIZE_16, TX_SIZE_16>> mockInterface;
    FlexCAN_T4<CAN1, RX_SIZE_16, TX_SIZE_16>& interface;

    Mock<AnalogInput> mockClutchRight;
    AnalogInput& clutchRight;

    Can can;

    CanTest() :
        interface(mockInterface.get()),
        clutchRight(mockClutchRight.get()),
        can(interface, clutchRight) {

    }

    void SetUp() override {
        When(Method(mockInterface, begin)).AlwaysReturn();
        When(Method(mockInterface, setBaudRate)).AlwaysReturn();
        When(Method(mockInterface, write)).AlwaysReturn(1);

        When(Method(mockClutchRight, travel)).AlwaysReturn(0);
        When(Method(ArduinoFake(), analogRead)).AlwaysReturn(0);

        can.begin();
    }

    void TearDown() override {
        ArduinoFakeReset();
    }
};

TEST_F(CanTest, begin) {
    Verify(Method(mockInterface, begin)).Once();
    Verify(Method(mockInterface, setBaudRate).Using(1000000)).Once();
}

TEST_F(CanTest, broadcastUp) {
    const CAN_message_t reference = {
        .id = 0x65cu,
        .buf = {0b00000001, 0x0F, 0xA0, 0x17, 0x70, 0x00, 0x00, 0x00}
    };

    CAN_message_t written;

    When(Method(mockInterface, write)).Do([&](const CAN_message_t& msg) -> int {
        written = msg;
        return 1;
    });

    can.broadcast(true, false, 40, 60);
    ASSERT_EQ(written, reference);
}

TEST_F(CanTest, broadcastDown) {
    const CAN_message_t reference = {
        .id = 0x65cu,
        .buf = {0b00000010, 0x0F, 0xA0, 0x17, 0x70, 0x00, 0x00, 0x00}
    };

    CAN_message_t written;

    When(Method(mockInterface, write)).Do([&](const CAN_message_t& msg) -> int {
        written = msg;
        return 1;
    });

    can.broadcast(false, true, 40, 60);
    ASSERT_EQ(written, reference);
}

TEST_F(CanTest, broadcastMin) {
    const CAN_message_t reference = {
        .id = 0x65cu,
        .buf = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
    };

    CAN_message_t written;

    When(Method(mockInterface, write)).Do([&](const CAN_message_t& msg) -> int {
        written = msg;
        return 1;
    });

    can.broadcast(false, false, -1, -1);
    ASSERT_EQ(written, reference);
}

TEST_F(CanTest, broadcastMax) {
    const CAN_message_t reference = {
        .id = 0x65cu,
        .buf = {0x00, 0x27, 0x10, 0x27, 0x10, 0x00, 0x00, 0x00}
    };

    CAN_message_t written;

    When(Method(mockInterface, write)).Do([&](const CAN_message_t& msg) -> int {
        written = msg;
        return 1;
    });

    can.broadcast(false, false, 101, 101);
    ASSERT_EQ(written, reference);
}

#endif
