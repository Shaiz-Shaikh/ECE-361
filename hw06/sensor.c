#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include "sensor.h"
#include "iom361_r4.h"

#define FIELD_MASK 0xFFFFFu     /* bits 19..0 */
#define SCALE      1048576.0    /* 2**20 */

/* Rounds to one decimal place. */
static double round_tenth(double x)
{
    return round(x * 10.0) / 10.0;
}

int sensor_read(uint32_t *base, time_t now, reading_t *r)
{
    int rc = 0;
    uint32_t st;
    uint32_t srh;
    double temp;
    double rh;

    if (base == NULL || r == NULL)
        return 1;

    st = iom361_readReg(base, TEMP_REG, &rc);
    if (rc != 0)
        return rc;
    srh = iom361_readReg(base, HUMID_REG, &rc);
    if (rc != 0)
        return rc;

    st &= FIELD_MASK;
    srh &= FIELD_MASK;
    temp = ((double)st / SCALE) * 200.0 - 50.0;
    rh = ((double)srh / SCALE) * 100.0;

    /* Write the outputs only now, after both reads and the math worked. */
    r->t = now;
    r->temp_c = (float)round_tenth(temp);
    r->rh_pct = (float)round_tenth(rh);
    return 0;
}
