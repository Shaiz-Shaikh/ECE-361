# SPEC: Weather Station (Project 31)

This file is a first draft. The transition table, the state machine drawing,
and the remaining sections come in later weeks.

## States

| State   | Meaning                       |
|---------|-------------------------------|
| CALM    | no heat risk (initial state)  |
| WATCH   | heat possible                 |
| WARNING | dangerous heat                |
| SEVERE  | extreme heat                  |

## Events

The device reacts to these events. A reading is classified by the operating
rules: WATCH at or above 30.0 C, WARNING at or above 35.0 C or at or above
32.0 C with 70 %RH or more, SEVERE at or above 40.0 C.

| Event              | When it happens                                      |
|--------------------|------------------------------------------------------|
| EV_CALM_READING    | a reading that meets no heat condition               |
| EV_WATCH_READING   | a reading that meets the WATCH condition only        |
| EV_WARNING_READING | a reading that meets the WARNING condition           |
| EV_SEVERE_READING  | a reading that meets the SEVERE condition            |
| EV_ACK_PRESS       | the ACK switch changes from 0 to 1 (a press)         |

The rule that a level is lowered only after 30 consecutive readings below its
entry condition will be handled when the transition table is designed.
