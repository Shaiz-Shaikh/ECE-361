#include <math.h>
#include <stddef.h>
#include "sensor.h"
#include "iom361_r4.h"

#define RAW_MASK  0xFFFFFu      /* keeps the lower 20 bits */
#define RAW_SCALE 1048576.0     /* 2 to the power 20 */

static double round_tenth(double x)
{
    return round(x * 10.0) / 10.0;
}

int sensor_read(uint32_t *base, float *temp_c, float *rh_pct)
{
    int rc = 0;

    if (base == NULL || temp_c == NULL || rh_pct == NULL) {
        return 1;
    }

    uint32_t raw_t = iom361_readReg(base, TEMP_REG, &rc);
    if (rc != 0) {
        return rc;
    }

    uint32_t raw_h = iom361_readReg(base, HUMID_REG, &rc);
    if (rc != 0) {
        return rc;
    }

    double t = (double)(raw_t & RAW_MASK) / RAW_SCALE * 200.0 - 50.0;
    double h = (double)(raw_h & RAW_MASK) / RAW_SCALE * 100.0;

    *temp_c = (float)round_tenth(t);
    *rh_pct = (float)round_tenth(h);
    return 0;
}
