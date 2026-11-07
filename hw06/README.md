# HW6: Replay a scenario (sensor, panel, scenario loader)

`./replay scenario.txt` replays a scenario file through the iom361 emulator
and prints one trace line per tick. The device is simple: a press of switch k
toggles LED k.

## Build and test

    make          # builds ./replay (gcc -std=c11 -Wall -Wextra, no warnings)
    make test     # unit tests (tests/test_hw06.c), then the regression test
    make clean    # removes everything that was built

Run it: `./replay scenarios/press.txt`

## Files

- `sensor.h`, `sensor.c`: `reading_t` and `sensor_read(base, now, &r)`. Reads
  TEMP_REG and HUMID_REG, keeps bits 19..0, converts with the formulas in
  iom361_r4.h, rounds to 0.1. A failed read changes nothing. No printf and no
  `_iom361_set_...` calls.
- `panel.h`, `panel.c`: `panel_pressed`, `panel_released` (bit operations on
  two switch values), and `panel_led`, `panel_toggle`. The LED functions read
  LEDS_REG, change only bit n, and write it back, so the other LEDs keep
  their state. Writing a value with only bit n set would turn all the others off.
- `scenario.h`, `scenario.c`: loads a scenario file into a fixed array of at
  most 1000 lines (no malloc). `scenario_load` returns 0, or the number of the
  first wrong line. `scenario_at` gives the line in force at a tick, so values
  hold between lines.
- `main.c`: the loop. For every tick from 0 to the last: set the world from the
  line in force, read the sensor and the switches through the registers, toggle
  LED k for every switch k just pressed, print the trace line.
- `iom361_r4.h`, `iom361_r4.c`: the emulator, copied from the course.
- `tests/test_hw06.c`: unit tests (PASS or FAIL per test, a summary, a nonzero
  exit status on failure). `tests/*.txt` are small scenario files for the
  loader tests, some of them wrong on purpose.
- `scenarios/` and `expected/`: two scenarios and their expected traces.
  `regress.sh` runs `./replay` on each scenario and compares with `diff`.

## Scenario format

`#` starts a comment. The first line that is not a comment is
`start <unix timestamp>`. Every line after it is `tick temp rh switches`, with
the switches in hexadecimal and the ticks strictly increasing. Between lines the
values hold.

## Trace line

    2026-10-06 08:00:02 temp=21.5 rh=40.0 switches=0x1 pressed=0x1 leds=0x1

The time is UTC (gmtime and strftime), so a trace is the same on every computer.

## Decisions

- A tick before the first line has no line in force: the sensor and switches
  keep their starting values (23.5 C, 75 %RH, switches 0).
- A bad scenario file stops the program with a message on stderr and exit
  status 1. `scenario_load` returns -1 if the file cannot be opened.
- The expected traces were checked by hand once before they were saved.
