#ifndef CHECK_H
#define CHECK_H

#include <stdio.h>
#include "lock.h"

/* fsm_check
   Takes:   a transition table, the start state, and a stream to print to.
   Returns: how many problems it found (0 means the table is fine).
   Prints:  one line per problem to out: an entry left out (A_UNDEFINED),
            a next state out of range, a state that cannot be reached from
            start (breadth-first search over the table), and a state with
            no way out (every event leaves it where it is).
   Assumes: start is a valid state and out is not NULL. */
int fsm_check(const transition_t table[N_STATES][N_EVENTS],
              state_t start, FILE *out);

#endif
