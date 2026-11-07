#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "iom361_r4.h"
#include "panel.h"
#include "scenario.h"
#include "sensor.h"

#define NUM_SWITCHES 4
#define NUM_LEDS     4

/* Prints one trace line: the time in UTC, the reading, the switch register,
   the switches just pressed, and the LED register. */
static void print_tick(const reading_t *r, uint32_t sw, uint32_t pressed,
                       uint32_t leds)
{
    char stamp[32] = "?";
    struct tm *tmv = gmtime(&r->t);

    if (tmv != NULL)
        strftime(stamp, sizeof stamp, "%Y-%m-%d %H:%M:%S", tmv);
    printf("%s temp=%.1f rh=%.1f switches=0x%x pressed=0x%x leds=0x%x\n",
           stamp, r->temp_c, r->rh_pct, (unsigned)sw, (unsigned)pressed,
           (unsigned)leds);
}

int main(int argc, char *argv[])
{
    static scenario_t sc;
    uint32_t *base;
    uint32_t before = 0;            /* the switches at the previous tick */
    int rc = 0;
    int status;
    int tick;
    int last;

    if (argc != 2) {
        fprintf(stderr, "usage: %s scenario.txt\n", argv[0]);
        return 1;
    }
    status = scenario_load(argv[1], &sc);
    if (status == -1) {
        fprintf(stderr, "%s: cannot read the file\n", argv[1]);
        return 1;
    }
    if (status > 0) {
        fprintf(stderr, "%s: line %d is wrong\n", argv[1], status);
        return 1;
    }

    base = iom361_initialize(NUM_SWITCHES, NUM_LEDS, &rc);
    if (rc != 0) {
        fprintf(stderr, "iom361_initialize failed (code %d)\n", rc);
        return 1;
    }
    iom361_set_display(false);      /* no LED display mixed into the trace */

    last = scenario_last_tick(&sc);
    for (tick = 0; tick <= last; tick++) {
        const scen_line_t *ln = scenario_at(&sc, tick);
        reading_t r;
        uint32_t sw;
        uint32_t pressed;
        uint32_t leds;
        int k;

        if (ln != NULL) {                           /* set the world */
            _iom361_set_sensor1(ln->temp_c, ln->rh_pct);
            _iom361_set_switches(ln->switches);
        }

        rc = sensor_read(base, sc.start + tick, &r);
        if (rc != 0) {
            fprintf(stderr, "tick %d: sensor_read failed (code %d)\n", tick, rc);
            return 1;
        }
        sw = iom361_readReg(base, SWITCHES_REG, &rc);
        if (rc != 0) {
            fprintf(stderr, "tick %d: reading the switches failed (code %d)\n",
                    tick, rc);
            return 1;
        }

        pressed = panel_pressed(sw, before);
        before = sw;
        for (k = 0; k < NUM_SWITCHES; k++) {
            if (pressed & (1u << k)) {
                rc = panel_toggle(base, k);
                if (rc != 0) {
                    fprintf(stderr, "tick %d: panel_toggle failed (code %d)\n",
                            tick, rc);
                    return 1;
                }
            }
        }

        leds = iom361_readReg(base, LEDS_REG, &rc);
        if (rc != 0) {
            fprintf(stderr, "tick %d: reading the LEDs failed (code %d)\n",
                    tick, rc);
            return 1;
        }
        print_tick(&r, sw, pressed, leds);
    }
    return 0;
}
