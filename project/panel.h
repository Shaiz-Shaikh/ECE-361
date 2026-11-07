#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>

/*
 * panel: the switches and LEDs of iom361. Bit k of a switch value is
 * switch k; bit k of the LED register is LED k.
 */

/* panel_pressed
 * Takes:   the switch values now and before (one bit per switch).
 * Returns: the switches that went from off to on: bit k is 1 only if
 *          switch k is on now and was off before. */
uint32_t panel_pressed(uint32_t now, uint32_t before);

/* panel_released
 * Takes:   the switch values now and before.
 * Returns: the switches that went from on to off: bit k is 1 only if
 *          switch k is off now and was on before. */
uint32_t panel_released(uint32_t now, uint32_t before);

/* panel_led
 * Takes:   base, the register block from iom361_initialize; n, the LED
 *          number; on, nonzero to turn it on and 0 to turn it off.
 * Does:    read-modify-write of LEDS_REG: reads the register, changes only
 *          bit n, and writes it back, so every other LED keeps its state.
 * Returns: 0 on success; -1 and changes nothing if n is not 0 to 31; the
 *          iom361 return code if a read or write fails.
 * Assumes: base is the pointer returned by iom361_initialize. */
int panel_led(uint32_t *base, int n, int on);

/* panel_toggle
 * Takes:   base and the LED number n.
 * Does:    flips LED n (on becomes off, off becomes on) the same way, and
 *          leaves every other LED alone.
 * Returns: the same as panel_led. */
int panel_toggle(uint32_t *base, int n);

#endif
