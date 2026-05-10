#pragma once
#include <mcp2515.h>

#define INIT_FRAME(message) \
    can_frame message##_frame; \
    message##_frame.can_id = message##_frame_id | (message##_is_extended ? CAN_EFF_FLAG : 0); \
    message##_frame.can_dlc = message##_length
