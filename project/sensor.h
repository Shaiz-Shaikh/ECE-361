#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/*
 * sensor: reads the temperature and humidity registers of iom361, converts
 * the raw 20-bit values to degrees C and %RH, and rounds them to 0.1.
 * Will define reading_t (timestamp, temperature, humidity).
 */

/* sensor_read
 * Takes:   base, the register block returned by iom361_initialize;
 *          temp_c and rh_pct, where to store the results.
 * Does:    reads TEMP_REG and HUMID_REG, converts them to degrees C and
 *          %RH, rounds both to 0.1, and stores them in *temp_c and *rh_pct.
 * Returns: 0 on success. If a register read fails, returns the iom361
 *          return code and changes nothing. Returns 1 and changes nothing
 *          if any pointer is NULL.
 * Assumes: base is the pointer returned by iom361_initialize. */
int sensor_read(uint32_t *base, float *temp_c, float *rh_pct);

#endif
