#include "status.h"
#include "bits.h"

enum {
    HEAT_POS = 0,
    COOL_POS = 1,
    FAN_POS = 2,
    FAULT_POS = 3,
    MODE_POS = 4,
    RESERVED_POS = 7,
    SETPOINT_POS = 8,

    FLAG_WIDTH = 1,
    MODE_WIDTH = 3,
    SETPOINT_WIDTH = 8
};

status_t status_unpack(uint16_t word)
{
    status_t status;
    uint32_t raw_mode;

    status.heat = get_field(word, HEAT_POS, FLAG_WIDTH);
    status.cool = get_field(word, COOL_POS, FLAG_WIDTH);
    status.fan = get_field(word, FAN_POS, FLAG_WIDTH);
    status.fault = get_field(word, FAULT_POS, FLAG_WIDTH);

    raw_mode = get_field(word, MODE_POS, MODE_WIDTH);

    if (raw_mode <= MODE_FAN_ONLY) {
        status.mode = (mode_t)raw_mode;
    } else {
        status.mode = MODE_INVALID;
    }

    status.reserved = get_field(word, RESERVED_POS, FLAG_WIDTH);

    status.setpoint = sign_extend(
        get_field(word, SETPOINT_POS, SETPOINT_WIDTH),
        SETPOINT_WIDTH
    );

    return status;
}
