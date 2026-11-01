# Bonus: table checker

`fsm_check(table, start, out)` in `check.c` looks at a transition table and
prints one line for each problem it finds. It returns the number of problems.
It checks four things:

1. An entry left out (its action is still A_UNDEFINED, which is 0).
2. A next state that is out of range.
3. A state that cannot be reached from the start state. It uses a
   breadth-first search: start at the start state, put each new state in a
   queue, and mark it as reached.
4. A state with no way out (no event leads to a different state).

Run it with `make bonus`. On the real lock_table it finds 0 problems.

## Broken copy 1: GOT2 + RIGHT left out

    entry left out: GOT2 + RIGHT
    state cannot be reached from LOCKED: OPEN

2 problems. This is a real bug for a lock because the last right digit does
nothing. Nobody could ever open the door. The second line is a result of the
first one: since the entry is left out, OPEN cannot be reached from LOCKED.

## Broken copy 2: GOT1 + RIGHT goes to state 7

    next state out of range: GOT1 + RIGHT -> 7
    state cannot be reached from LOCKED: GOT2
    state cannot be reached from LOCKED: OPEN

3 problems. This is a real bug because state 7 does not exist. In real code
the lock would read memory outside the tables, so the lock could crash or
behave in a random way after the second digit. Also the code can never be
finished, because GOT2 and OPEN cannot be reached anymore.

## Broken copy 3: OPEN never locks again

    state has no way out: OPEN

1 problem. This is a real bug because once the door is open, closing it and
the timer both leave the lock in OPEN. The lock stays unlocked forever,
which is a security hole. Nothing else is wrong with the table, so only the
dead-end check can find this one.
