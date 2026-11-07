#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "fsm.h"
#include "iom361_r4.h"
#include "panel.h"
#include "scenario.h"
#include "sensor.h"
#include "store.h"

#define NUM_SWITCHES 1          /* switch 0 is ACK */
#define NUM_LEDS     4          /* WATCH_LAMP, WARNING_LAMP, SEVERE_LAMP, BUZZER */
#define ACK_SWITCH   0x1u

/* RGB color of each state: red, green, blue duty cycles (0 to 255). */
static const uint8_t colors[N_FSM_STATES][3] = {
    [CALM]    = {   0, 255, 0 },        /* green  */
    [WATCH]   = { 255, 255, 0 },        /* yellow */
    [WARNING] = { 255, 128, 0 },        /* orange */
    [SEVERE]  = { 255,   0, 0 },        /* red    */
};

/* Writes the LEDs and the RGB LED for the current state. Returns 0, or the
   iom361 return code of a failed write. */
static int write_outputs(uint32_t *base, const fsm_t *f)
{
    const uint8_t *c = colors[f->state];
    int rc = 0;

    iom361_writeReg(base, LEDS_REG, fsm_leds(f), &rc);
    if (rc != 0)
        return rc;
    iom361_writeReg(base, RGB_LED_REG, build_rgb_reg(1, c[0], c[1], c[2]), &rc);
    return rc;
}

/* Prints one trace line: the time in UTC, the state, the reading, the LED
   register, and the status word. */
static void print_tick(const reading_t *r, const fsm_t *f, uint32_t leds)
{
    char stamp[32] = "?";
    struct tm *tmv = gmtime(&r->t);

    if (tmv != NULL)
        strftime(stamp, sizeof stamp, "%Y-%m-%d %H:%M:%S", tmv);
    printf("%s state=%s temp=%.1f rh=%.1f leds=0x%x status=0x%08x\n",
           stamp, fsm_state_name(f->state), r->temp_c, r->rh_pct,
           (unsigned)leds, (unsigned)fsm_status(f));
}

int main(int argc, char *argv[])
{
    static scenario_t sc;
    uint32_t *base;
    uint32_t before = 0;            /* the switches at the previous tick */
    store_t *store;
    fsm_t f;
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

    store = store_create();
    if (store == NULL) {
        fprintf(stderr, "cannot create the store\n");
        return 1;
    }
    fsm_init(&f);

    last = scenario_last_tick(&sc);
    for (tick = 0; tick <= last; tick++) {
        const scen_line_t *ln = scenario_at(&sc, tick);
        record_t rec;
        reading_t r;
        uint32_t sw;
        uint32_t leds;
        int ack;

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

        ack = (panel_pressed(sw, before) & ACK_SWITCH) != 0;
        before = sw;
        fsm_tick(&f, r.temp_c, r.rh_pct, ack);

        rc = write_outputs(base, &f);
        if (rc != 0) {
            fprintf(stderr, "tick %d: writing the outputs failed (code %d)\n",
                    tick, rc);
            return 1;
        }

        rec.reading = r;
        rec.state = (int)f.state;
        rec.status = fsm_status(&f);
        store_insert(store, &rec);      /* a full store just stops filling */

        leds = iom361_readReg(base, LEDS_REG, &rc);
        if (rc != 0) {
            fprintf(stderr, "tick %d: reading the LEDs failed (code %d)\n",
                    tick, rc);
            return 1;
        }
        print_tick(&r, &f, leds);
    }
    store_destroy(store);
    return 0;
}
