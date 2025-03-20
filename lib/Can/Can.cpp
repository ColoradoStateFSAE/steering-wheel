#include "Can.h"

using namespace std;

void Can::begin() {
    mcp.begin(1000000);
}

void Can::broadcast(bool up, bool down, float clutchRight, float clutchLeft) {
    clutchRight = constrain(clutchRight, 0.0f, 100.0f);
    clutchLeft = constrain(clutchLeft, 0.0f, 100.0f);

    mcp.beginPacket(TCS_SHIFT_CONTROLLER_FRAME_ID);
    uint8_t msg[8] = {0};
    
    tcs_shift_controller_t buf;
    buf.up = tcs_shift_controller_up_encode(up);
    buf.down = tcs_shift_controller_down_encode(down);
    buf.clutch_right = tcs_shift_controller_clutch_right_encode(clutchRight);
    buf.clutch_left = tcs_shift_controller_clutch_left_encode(clutchLeft);
    tcs_shift_controller_pack(msg, &buf, sizeof(msg));
    
    mcp.write(msg, sizeof(msg));
    mcp.endPacket();
}
