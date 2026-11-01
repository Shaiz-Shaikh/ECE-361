# HW5: Combination lock (finite state machine)

A lock with the code 3-1-4, written as a transition table. One table lookup
in `lock_step` replaces a chain of ifs.

## Build and test

    make          # builds ./lock
    make test     # builds and runs the tests (PASS or FAIL per test)
    make clean    # removes everything that was built

Try it: `./lock < data/codes.txt`

Characters: a digit is a keypad press, `c` is the door closing, `t` is the
5-second timer running out. Other characters (spaces, newlines) are ignored.

## States, events, actions

- States: LOCKED (nothing typed), GOT1 (3 typed), GOT2 (3-1 typed), OPEN.
- Events: RIGHT (the next digit of the code), WRONG (any other digit),
  CLOSE (door closes), TIMEOUT (timer runs out).
- Actions: NONE, UNLOCK, LOCK, BEEP.

## State diagram

                RIGHT / none     RIGHT / none     RIGHT / UNLOCK
      LOCKED ----------------> GOT1 ------------> GOT2 ----------------> OPEN
        ^ ^                     |                  |                      |
        | |   WRONG / BEEP      |                  |                      |
        | +---------------------+------------------+                      |
        |     TIMEOUT / none (from GOT1 and GOT2)                          |
        +------------------------------------------------------------------+
              CLOSE / LOCK and TIMEOUT / LOCK (from OPEN)

    Staying in the same state:
      LOCKED: CLOSE, TIMEOUT -> LOCKED / none; WRONG -> LOCKED / BEEP
      GOT1: CLOSE -> GOT1 / none      GOT2: CLOSE -> GOT2 / none
      OPEN: RIGHT, WRONG -> OPEN / none

## Transition table

Each box is `next state / action`.

| State  | RIGHT         | WRONG         | CLOSE         | TIMEOUT       |
|--------|---------------|---------------|---------------|---------------|
| LOCKED | GOT1 / NONE   | LOCKED / BEEP | LOCKED / NONE | LOCKED / NONE |
| GOT1   | GOT2 / NONE   | LOCKED / BEEP | GOT1 / NONE   | LOCKED / NONE |
| GOT2   | OPEN / UNLOCK | LOCKED / BEEP | GOT2 / NONE   | LOCKED / NONE |
| OPEN   | OPEN / NONE   | OPEN / NONE   | LOCKED / LOCK | LOCKED / LOCK |

## Files

- `lock.h`, `lock.c`: the types, `lock_table`, `lock_step`, `lock_event`, and
  the name functions.
- `main.c`: reads characters from standard input, prints one line per event.
- `data/codes.txt`: sample input.
- `tests/test_lock.c`: tests for all 16 table entries, `lock_event`, and the
  sequences 3 1 4, 3 3 1 4, 3 1 t 4, and 3 1 4 c.

## Bonus: table checker

`check.h` and `check.c` have `fsm_check`, which finds left-out entries,
out-of-range next states, unreachable states (breadth-first search), and
states with no way out. `make bonus` runs `tests/test_check.c`: the real
table must give 0 problems, and three broken copies must be caught. The
output and the reasons are in `BONUS.md`.
