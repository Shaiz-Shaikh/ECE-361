#ifndef STATUS_H
#define STATUS_H

#include <stdint.h>

typedef enum {
    MODE_OFF = 0,
    MODE_HEAT = 1,
    MODE_COOL = 2,
    MODE_AUTO = 3,
    MODE_FAN_ONLY = 4,
    MODE_INVALID = -1
} mode_t;

typedef struct {
    uint8_t heat;
    uint8_t cool;
    uint8_t fan;
    uint8_t fault;
    mode_t mode;
    uint8_t reserved;
    int32_t setpoint;
} status_t;

status_t status_unpack(uint16_t word);

#endif

