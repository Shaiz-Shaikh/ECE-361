#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <time.h>

/*
 * sensor: reads the temperature and humidity registers of iom361, converts
 * the raw 20-bit values to degrees C and %RH, and rounds them to 0.1.
 */

/* reading_t: one measurement. t is when it was taken (seconds since
   1 Jan 1970, UTC), temp_c is in degrees C, rh_pct in %RH. */
typedef struct {
    time_t t;
    float  temp_c;
    float  rh_pct;
} reading_t;

/* sensor_read
 * Takes:   base, the register block returned by iom361_initialize;
 *          now, the timestamp for this reading; r, where to store it.
 * Does:    reads TEMP_REG and HUMID_REG, keeps bits 19..0 of each, converts
 *          them with the formulas in iom361_r4.h, rounds both to 0.1, and
 *          stores them with the timestamp now in *r.
 * Returns: 0 on success. If a register read fails, returns the iom361
 *          return code and changes nothing. Returns 1 and changes nothing
 *          if base or r is NULL.
 * Assumes: base is the pointer returned by iom361_initialize. */
int sensor_read(uint32_t *base, time_t now, reading_t *r);

#endif
