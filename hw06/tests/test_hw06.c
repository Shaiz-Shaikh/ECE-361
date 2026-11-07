#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "iom361_r4.h"
#include "panel.h"
#include "scenario.h"
#include "sensor.h"

static int passed = 0;
static int failed = 0;

/* Prints PASS or FAIL for one test and counts it. */
static void check(const char *name, int ok)
{
    if (ok) {
        printf("PASS: %s\n", name);
        passed++;
    } else {
        printf("FAIL: %s\n", name);
        failed++;
    }
}

static int near(float a, float b)
{
    return fabsf(a - b) < 0.001f;
}

static void test_sensor(void)
{
    int rc = 0;
    uint32_t *base = iom361_initialize(4, 4, &rc);
    uint32_t bogus[8] = { 0 };      /* not the block iom361_initialize gave */
    reading_t r;

    iom361_set_display(false);
    _iom361_set_sensor1(91.0f, 50.0f);
    rc = sensor_read(base, 1234, &r);
    check("sensor: the read succeeds", rc == 0);
    check("sensor: 91.0 C reads back as 91.0", near(r.temp_c, 91.0f));
    check("sensor: 50.0 %RH reads back as 50.0", near(r.rh_pct, 50.0f));
    check("sensor: the timestamp is stored", r.t == 1234);

    r.t = 7;
    r.temp_c = 1.5f;
    r.rh_pct = 2.5f;
    rc = sensor_read(bogus, 9999, &r);
    check("sensor: a failed read returns an error code", rc != 0);
    check("sensor: a failed read changes nothing",
          r.t == 7 && near(r.temp_c, 1.5f) && near(r.rh_pct, 2.5f));
    check("sensor: a NULL reading returns 1", sensor_read(base, 0, NULL) == 1);
}

static uint32_t read_leds(uint32_t *base)
{
    int rc = 0;

    return iom361_readReg(base, LEDS_REG, &rc);
}

static void test_panel(void)
{
    int rc = 0;
    uint32_t *base = iom361_initialize(4, 4, &rc);

    iom361_set_display(false);

    check("panel: a press is seen by panel_pressed",
          panel_pressed(0x1, 0x0) == 0x1);
    check("panel: a press is not a release",
          panel_released(0x1, 0x0) == 0x0);
    check("panel: a release is seen by panel_released",
          panel_released(0x0, 0x1) == 0x1);
    check("panel: a release is not a press",
          panel_pressed(0x0, 0x1) == 0x0);
    check("panel: a held switch is not pressed",
          panel_pressed(0x1, 0x1) == 0x0);
    check("panel: a held switch is not released",
          panel_released(0x1, 0x1) == 0x0);
    check("panel: only the switch that changed is pressed",
          panel_pressed(0x6, 0x2) == 0x4);

    check("panel: LEDs start dark", read_leds(base) == 0x0);
    check("panel: panel_led turns LED 1 on",
          panel_led(base, 1, 1) == 0 && read_leds(base) == 0x2);
    check("panel: turning LED 3 on keeps LED 1",
          panel_led(base, 3, 1) == 0 && read_leds(base) == 0xA);
    check("panel: turning LED 1 off keeps LED 3",
          panel_led(base, 1, 0) == 0 && read_leds(base) == 0x8);
    check("panel: panel_toggle turns LED 0 on and keeps LED 3",
          panel_toggle(base, 0) == 0 && read_leds(base) == 0x9);
    check("panel: panel_toggle turns LED 0 off and keeps LED 3",
          panel_toggle(base, 0) == 0 && read_leds(base) == 0x8);
    check("panel: panel_toggle turns LED 3 off",
          panel_toggle(base, 3) == 0 && read_leds(base) == 0x0);

    check("panel: panel_led(-1) returns -1", panel_led(base, -1, 1) == -1);
    check("panel: panel_led(32) returns -1", panel_led(base, 32, 1) == -1);
    check("panel: panel_toggle(-1) returns -1", panel_toggle(base, -1) == -1);
    check("panel: a bad LED number changes nothing", read_leds(base) == 0x0);
}

static void test_scenario(void)
{
    static scenario_t sc;
    const scen_line_t *ln;

    check("scenario: a good file loads",
          scenario_load("tests/ok.txt", &sc) == 0);
    check("scenario: comments and blank lines are skipped (3 lines kept)",
          sc.n == 3);
    check("scenario: the start time is read", sc.start == 1000);
    check("scenario: the last tick is 25", scenario_last_tick(&sc) == 25);

    ln = scenario_at(&sc, 0);
    check("scenario: tick 0 uses the line for tick 0",
          ln != NULL && ln->tick == 0 && near(ln->temp_c, 20.0f));
    ln = scenario_at(&sc, 9);
    check("scenario: values hold between lines (tick 9 uses tick 0)",
          ln != NULL && ln->tick == 0 && near(ln->temp_c, 20.0f));
    ln = scenario_at(&sc, 10);
    check("scenario: a new line takes over at its own tick",
          ln != NULL && ln->tick == 10 && near(ln->temp_c, 30.5f)
          && near(ln->rh_pct, 60.0f) && ln->switches == 0x1);
    ln = scenario_at(&sc, 24);
    check("scenario: values hold again (tick 24 uses tick 10)",
          ln != NULL && ln->tick == 10);
    ln = scenario_at(&sc, 100);
    check("scenario: after the last line, the last line holds",
          ln != NULL && ln->tick == 25 && near(ln->temp_c, 31.0f));
    check("scenario: before the first line nothing is in force",
          scenario_at(&sc, -1) == NULL);

    check("scenario: no start line is reported at line 2",
          scenario_load("tests/nostart.txt", &sc) == 2);
    check("scenario: a missing field is reported at line 3",
          scenario_load("tests/missing.txt", &sc) == 3);
    check("scenario: ticks that do not increase are reported at line 4",
          scenario_load("tests/noincrease.txt", &sc) == 4);
    check("scenario: a file that does not exist returns -1",
          scenario_load("tests/nofile.txt", &sc) == -1);
}

int main(void)
{
    test_sensor();
    test_panel();
    test_scenario();

    printf("\n%d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
