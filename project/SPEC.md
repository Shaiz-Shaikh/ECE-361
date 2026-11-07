# SPEC: Weather Station (Project 31)

Device type D (monitor and logger), key K2 (temperature in 0.5 C steps),
structure S3 (circular buffer), report Q2 (top-k highest temperatures).

This file is updated as the project grows. The last section lists what is
still to be written.

## States

| State   | Level | Meaning                      |
|---------|-------|------------------------------|
| CALM    | 0     | no heat risk (initial state) |
| WATCH   | 1     | heat possible                |
| WARNING | 2     | dangerous heat               |
| SEVERE  | 3     | extreme heat                 |

## Events

The level of one reading follows the operating rules: level 3 at or above
40.0 C; level 2 at or above 35.0 C, or at or above 32.0 C with 70 %RH or more;
level 1 at or above 30.0 C; otherwise level 0. The temperature and humidity are
already rounded to 0.1 by `sensor_read` when the level is decided.

| Event   | When it happens                                                  |
|---------|------------------------------------------------------------------|
| E_L0    | a reading of level 0                                             |
| E_L1    | a reading of level 1                                             |
| E_L2    | a reading of level 2                                             |
| E_L3    | a reading of level 3                                             |
| E_LOWER | 30 readings in a row were below the level of the current state   |
| E_ACK   | the ACK switch (switch 0) changes from 0 to 1 (a press)          |

E_L0 to E_L3 are numbered 0 to 3, the same as the levels. In one tick, ACK (if
pressed) is handled first, then the event for the reading.

## Actions

A_NONE, A_BUZZER_ON (the buzzer starts sounding), A_BUZZER_OFF (the buzzer is
silenced). The buzzer is a flag kept next to the state in `fsm_t`.

## Transition table

Each box is next state / action. `fsm_step` is one lookup in this table.

| State   | E_L0            | E_L1            | E_L2                | E_L3                | E_LOWER         | E_ACK                   |
|---------|-----------------|-----------------|---------------------|---------------------|-----------------|-------------------------|
| CALM    | CALM / NONE     | WATCH / NONE    | WARNING / BUZZER_ON | SEVERE / BUZZER_ON  | CALM / NONE     | CALM / NONE             |
| WATCH   | WATCH / NONE    | WATCH / NONE    | WARNING / BUZZER_ON | SEVERE / BUZZER_ON  | CALM / NONE     | WATCH / NONE            |
| WARNING | WARNING / NONE  | WARNING / NONE  | WARNING / NONE      | SEVERE / BUZZER_ON  | WATCH / NONE    | WARNING / BUZZER_OFF    |
| SEVERE  | SEVERE / NONE   | SEVERE / NONE   | SEVERE / NONE       | SEVERE / NONE       | WARNING / NONE  | SEVERE / BUZZER_OFF     |

## State machine drawing

    Rising: a reading of a higher level takes the state straight to that level.

          E_L1               E_L2               E_L3
    CALM -------> WATCH -------> WARNING -------> SEVERE
      ^              |                |                |
      |              |  E_LOWER: 30 readings in a row   |
      +--------------+---------------+------------------+
        below the level, one level down at a time

    Also: CALM -> WARNING (E_L2), CALM -> SEVERE (E_L3), WATCH -> SEVERE (E_L3).
    Entering WARNING or SEVERE from below sounds the buzzer (A_BUZZER_ON).
    E_ACK never changes the state; in WARNING and SEVERE it silences the buzzer.

## Decisions where a rule was silent

1. **Where the event code lives.** `fsm_level` (reading to level) and
   `fsm_tick` (counter and events) are in `fsm.c`. The rule that classifies a
   reading is part of the behavior of the device, so it belongs with the
   state machine and is tested with it. `sensor.c` only talks to the hardware,
   and `main.c` stays a loop that connects the modules.
2. **ACK and a reading in the same tick.** ACK is handled first, so an alarm
   that starts in the same tick still sounds. If the reading came first, ACK
   would silence an alarm the user has not seen yet.
3. **ACK in CALM or WATCH** does nothing, because the buzzer is never sounding
   there. **ACK in WARNING or SEVERE** silences the buzzer and never changes
   the state. ACK with the buzzer already silent does nothing.
4. **A jump from CALM straight to SEVERE** (or to WARNING) is allowed and
   sounds the buzzer once.
5. **A reading at the same level as the state** is not "below" it, so it
   restarts the count of readings in a row.
6. **A reading below the level while in that level**, for example a level-2
   reading while the state is SEVERE, counts as below: it is not a rise, since
   the state is already higher. It adds one to the count of 30.
7. **Falling** is one level at a time. After 30 readings in a row below the
   level, the state drops one level and the count starts again at 0, so each
   level needs its own 30 readings (SEVERE to CALM takes at least 90 ticks).
8. **The buzzer after a fall** keeps sounding until ACK. A fall does not
   silence it.
9. **Rising into WATCH** does not sound the buzzer. Only a rise into WARNING or
   SEVERE does.
10. **Before the first line of a scenario** there is no line in force, so the
    sensor and the switches keep their starting values (23.5 C, 75 %RH,
    switches 0).
11. **Readings and thresholds.** The level is decided on the temperature and
    humidity rounded to 0.1, so a reading of 29.96 C is 30.0 C and counts as WATCH.
12. **A full store** (2048 records in the array version) stops taking records;
    the device keeps running.
13. **A bad scenario file** stops the program with a message on stderr
    (the number of the first wrong line) and exit status 1.

## record_t and the store (key K2)

`record_t` holds the reading (`reading_t`: the timestamp `t` as a `time_t`, the
temperature in C and the humidity in %RH, both floats), the state at that
moment (an int), and the status word (a `uint32_t`).

**Quantization.** The key of a record is `floor(temp * 2)`: the temperature in
steps of 0.5 C, rounded down. 21.2 C and 21.4 C both give 42. `record_compare`
compares only the keys, so two records with the same key compare as equal.

**Duplicates (sorted array, for now).** Every record is kept. A record whose
key already exists goes after the ones with that key, so equal keys stay in
arrival order. `store_find` returns the earliest record with that key. What
the tree does with duplicates will be decided in week 8 and written here.

## Status word

One `uint32_t`, packed with `set_field` and read with `get_field` (HW1).

| Bits  | Field        | Meaning                                         |
|-------|--------------|-------------------------------------------------|
| 1..0  | state        | CALM 0, WATCH 1, WARNING 2, SEVERE 3            |
| 2     | WATCH_LAMP   | 1 in state WATCH                                |
| 3     | WARNING_LAMP | 1 in state WARNING                              |
| 4     | SEVERE_LAMP  | 1 in state SEVERE                               |
| 5     | BUZZER       | 1 while the buzzer is sounding                  |
| 31..6 | unused       | 0                                               |

Bits 5..2 are the four LEDs of the LED register (bit 0 WATCH_LAMP, bit 1
WARNING_LAMP, bit 2 SEVERE_LAMP, bit 3 BUZZER) moved up by two. For example,
SEVERE with the buzzer sounding is 0x33; WATCH is 0x05.

## RGB color of each state

| State   | Color  | Red | Green | Blue |
|---------|--------|-----|-------|------|
| CALM    | green  | 0   | 255   | 0    |
| WATCH   | yellow | 255 | 255   | 0    |
| WARNING | orange | 255 | 128   | 0    |
| SEVERE  | red    | 255 | 0     | 0    |

The numbers are PWM duty cycles (0 to 255) passed to `build_rgb_reg`.

## The 2038 question

A signed 32-bit `time_t` counts up to 2,147,483,647 seconds after 1 January
1970, which is 03:14:07 UTC on 19 January 2038. One second later it overflows
(undefined behavior in C) and in practice wraps to -2,147,483,648, which is
13 December 1901. My device stores `start + tick` as a `time_t` in every
reading, so after that second the timestamps would jump back to 1901, the
trace would print the wrong date, and newer records would look older than
older ones. Anything that sorts or compares by time would break. The fix is a
64-bit `time_t`, which is what this computer uses; it holds dates for about
292 billion years.

## Still to write

- How the circular buffer keeps track of the oldest and the newest entry
  (when `aux.c` is written).
- The exact definition of the report (when `report.c` is written): the 5
  highest temperatures with tick and humidity, ties broken by the higher
  humidity, then by the earlier tick.
- What the tree does with duplicate keys (week 8).
- One paragraph that defends or criticizes the four given states.
- One paragraph that compares my circular buffer with one alternative from the
  reading, with the cost of each operation.
