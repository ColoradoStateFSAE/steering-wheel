#include "CanController.h"

using namespace std;

void CanController::begin() {
    mcp.reset();
    mcp.setBitrate(CAN_1000KBPS);
    mcp.setNormalMode();

    timers.setInterval([&]() {
        send_swc_clutch();
    }, SWC_CLUTCH_CYCLE_TIME_MS);

    timers.setInterval([&]() {
        send_haltech_io12a_avi();
    }, HALTECH_IO12A_AVI_CYCLE_TIME_MS);

    signals.onButton = [&](ShiftDirection direction) {
        sendShift(direction);
    };

    signals.onEncoder = [&](int button, int amount) {
        sendEncoder(button, amount);
    };
}

void CanController::update() {
    can_frame msg;
    if(mcp.readMessage(&msg) == MCP2515::ERROR_OK) {
        lastUpdateTime = millis();
    }

    signals.offline = millis() - lastUpdateTime >= 100;
    timers.handle();
}

void CanController::SEND_MESSAGE(swc_clutch) {
    ENCODE_SIGNAL(swc_clutch, clutch_left, signals.clutchLeft);
    ENCODE_SIGNAL(swc_clutch, clutch_right, signals.clutchRight);
    ENCODE_SIGNAL(swc_clutch, clutch_left_raw, signals.clutchLeftRaw);
    ENCODE_SIGNAL(swc_clutch, clutch_right_raw, signals.clutchRightRaw);

    INIT_FRAME(swc_clutch);
    PACK_MESSAGE(swc_clutch, swc_clutch_frame.data);
    mcp.sendMessage(&swc_clutch_frame);
}

void CanController::SEND_MESSAGE(haltech_io12a_avi) {
    float rotaryLeftVolts = signals.rotaryLeftVolts;
    float rotaryRightVolts = signals.rotaryRightVolts;

    if(rotaryLeftVolts < 0 || rotaryRightVolts < 0) return;

    ENCODE_SIGNAL(haltech_io12a_avi, avi1_voltage, rotaryLeftVolts);
    ENCODE_SIGNAL(haltech_io12a_avi, avi2_voltage, rotaryRightVolts);

    INIT_FRAME(haltech_io12a_avi);
    PACK_MESSAGE(haltech_io12a_avi, haltech_io12a_avi_frame.data);
    mcp.sendMessage(&haltech_io12a_avi_frame);
}


void CanController::sendShift(ShiftDirection direction) {
    INIT_MESSAGE(swc_buttons);
    ENCODE_SIGNAL(swc_buttons, shift, direction);

    INIT_FRAME(swc_buttons);
    PACK_MESSAGE(swc_buttons, swc_buttons_frame.data);
    mcp.sendMessage(&swc_buttons_frame);
}

void CanController::sendEncoder(bool button, int amount) {
    INIT_MESSAGE(swc_buttons);
    ENCODE_SIGNAL(swc_buttons, encoder_button, button);
    ENCODE_SIGNAL(swc_buttons, encoder_amount, amount);

    INIT_FRAME(swc_buttons);
    PACK_MESSAGE(swc_buttons, swc_buttons_frame.data);
    mcp.sendMessage(&swc_buttons_frame);
}
