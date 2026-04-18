#include "CanController.h"

using namespace std;

void CanController::begin() {
    mcp.reset();
    mcp.setBitrate(CAN_1000KBPS);
    mcp.setNormalMode();

    timers.setInterval([&]() {
        send_swc_clutch();
    }, SWC_CLUTCH_CYCLE_TIME_MS);

    signals.onButton = [&](ShiftDirection direction) {
        sendShift(direction);
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

void CanController::sendShift(ShiftDirection direction) {
    INIT_MESSAGE(swc_buttons);
    ENCODE_SIGNAL(swc_buttons, shift_direction, direction);

    INIT_FRAME(swc_buttons);
    PACK_MESSAGE(swc_buttons, swc_buttons_frame.data);
    mcp.sendMessage(&swc_buttons_frame);
}
