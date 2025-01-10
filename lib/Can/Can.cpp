#include "Can.h"

using namespace std;

void Can::begin() {
    interface.begin();
    interface.setBaudRate(1000000);
}

void Can::broadcast(bool up, bool down, float clutchRight, float clutchLeft) {
    clutchRight = constrain(clutchRight, 0.0f, 100.0f);
    clutchLeft = constrain(clutchLeft, 0.0f, 100.0f);
      
    CAN_message_t msg;
    msg.id = TCS_SHIFT_CONTROLLER_FRAME_ID;

    tcs_shift_controller_t buf;
    buf.up = tcs_shift_controller_up_encode(up);
    buf.down = tcs_shift_controller_down_encode(down);
    buf.clutch_right = tcs_shift_controller_clutch_right_encode(clutchRight);
    buf.clutch_left = tcs_shift_controller_clutch_left_encode(clutchLeft);
    tcs_shift_controller_pack(msg.buf, &buf, sizeof(msg.buf));

    interface.write(msg);
}
