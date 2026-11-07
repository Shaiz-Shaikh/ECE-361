#include <stdint.h>
#include "panel.h"
#include "iom361_r4.h"

uint32_t panel_pressed(uint32_t now, uint32_t before)
{
    return now & ~before;
}

uint32_t panel_released(uint32_t now, uint32_t before)
{
    return before & ~now;
}

int panel_led(uint32_t *base, int n, int on)
{
    int rc = 0;
    uint32_t leds;

    if (n < 0 || n > 31)
        return -1;

    leds = iom361_readReg(base, LEDS_REG, &rc);     /* read */
    if (rc != 0)
        return rc;

    if (on)                                         /* modify bit n only */
        leds |= (1u << n);
    else
        leds &= ~(1u << n);

    iom361_writeReg(base, LEDS_REG, leds, &rc);     /* write */
    return rc;
}

int panel_toggle(uint32_t *base, int n)
{
    int rc = 0;
    uint32_t leds;

    if (n < 0 || n > 31)
        return -1;

    leds = iom361_readReg(base, LEDS_REG, &rc);
    if (rc != 0)
        return rc;

    leds ^= (1u << n);                              /* flip bit n only */

    iom361_writeReg(base, LEDS_REG, leds, &rc);
    return rc;
}
