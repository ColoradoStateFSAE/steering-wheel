#pragma once
#ifdef PIO_UNIT_TESTING

#include <gtest/gtest.h>
#include <Arduino.h>
#include <mcp2515.h>
#include "CanController/CanController.h"

using namespace fakeit;
using namespace std;

class CanControllerTest : public ::testing::Test {
  protected:
    // MCP2515
    Mock<MCP2515> mockMcp;
    MCP2515& mcp = mockMcp.get();

    Signals signals;

    CanController canController = CanController(mcp, signals);

    can_frame mock_frame;
    unsigned long time = 0;

    void SetUp() override {
        // MCP2515
        When(Method(mockMcp, reset)).AlwaysReturn();
        When(Method(mockMcp, setBitrate)).AlwaysReturn();
        When(Method(mockMcp, setNormalMode)).AlwaysReturn();
        When(Method(mockMcp, sendMessage)).AlwaysReturn(MCP2515::ERROR_OK);
        When(Method(mockMcp, readMessage)).AlwaysReturn(MCP2515::ERROR_OK);

        When(Method(ArduinoFake(), millis)).AlwaysDo([&]() { return time; });

        canController.begin();
    }

    void TearDown() override {
        ArduinoFakeReset();
    }
};

TEST_F(CanControllerTest, begin) {
    Verify(
        Method(mockMcp, reset),
        Method(mockMcp, setBitrate).Using(CAN_1000KBPS),
        Method(mockMcp, setNormalMode)
    );
}

TEST_F(CanControllerTest, swc_buttons) {
    int count = 0;

    for(auto direction : {UP, DOWN}) {
        INIT_MESSAGE(swc_buttons);
        ENCODE_SIGNAL(swc_buttons, shift_direction, direction);
        INIT_FRAME(swc_buttons);
        PACK_MESSAGE(swc_buttons, swc_buttons_frame.data);

        When(Method(mockMcp, sendMessage)).AlwaysDo([&](const struct can_frame *frame) -> MCP2515::ERROR {
            if(*frame == swc_buttons_frame) count++;
            return MCP2515::ERROR_OK;
        });

        signals.onButton(direction);
    }

    ASSERT_EQ(count, 2);
}

TEST_F(CanControllerTest, swc_clutch) {
    signals.clutchLeft = 33.3;
    signals.clutchRight = 50.5;
    signals.clutchLeftRaw = 230;
    signals.clutchRightRaw = 980;

    INIT_MESSAGE(swc_clutch);
    ENCODE_SIGNAL(swc_clutch, clutch_left, signals.clutchLeft);
    ENCODE_SIGNAL(swc_clutch, clutch_right, signals.clutchRight);
    ENCODE_SIGNAL(swc_clutch, clutch_left_raw, signals.clutchLeftRaw);
    ENCODE_SIGNAL(swc_clutch, clutch_right_raw, signals.clutchRightRaw);
    INIT_FRAME(swc_clutch);
    PACK_MESSAGE(swc_clutch, swc_clutch_frame.data);

    int count = 0;

    When(Method(mockMcp, sendMessage)).AlwaysDo([&](const struct can_frame *frame) -> MCP2515::ERROR {
        if(*frame == swc_clutch_frame) count++;
        return MCP2515::ERROR_OK;
    });

    for(int i = 0; i < 20; i++) {
        time += SWC_CLUTCH_CYCLE_TIME_MS;
        canController.update();
    }

    ASSERT_EQ(count, 20);
}

#endif
