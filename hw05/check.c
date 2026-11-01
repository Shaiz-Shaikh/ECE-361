#include <stdio.h>
#include "check.h"

/* Problems 1 and 2: looks at every entry. Reports an entry that was left
   out (A_UNDEFINED) and a next state that is out of range. */
static int check_entries(const transition_t table[N_STATES][N_EVENTS],
                         FILE *out)
{
    int problems = 0;
    int s, e;

    for (s = 0; s < N_STATES; s++) {
        for (e = 0; e < N_EVENTS; e++) {
            const transition_t *t = &table[s][e];

            if (t->action == A_UNDEFINED) {
                fprintf(out, "entry left out: %s + %s\n",
                        state_name((state_t)s), event_name((event_t)e));
                problems++;
            }
            if ((int)t->next < 0 || (int)t->next >= N_STATES) {
                fprintf(out, "next state out of range: %s + %s -> %d\n",
                        state_name((state_t)s), event_name((event_t)e),
                        (int)t->next);
                problems++;
            }
        }
    }
    return problems;
}

/* Problem 3: finds the states that cannot be reached from start, with a
   breadth-first search. Entries that were left out or point out of range
   are not followed. */
static int check_reachable(const transition_t table[N_STATES][N_EVENTS],
                           state_t start, FILE *out)
{
    int reached[N_STATES] = { 0 };
    int queue[N_STATES];
    int head = 0;
    int tail = 0;
    int problems = 0;
    int s, e;

    reached[start] = 1;
    queue[tail++] = (int)start;

    while (head < tail) {
        int cur = queue[head++];

        for (e = 0; e < N_EVENTS; e++) {
            const transition_t *t = &table[cur][e];
            int next = (int)t->next;

            if (t->action == A_UNDEFINED || next < 0 || next >= N_STATES)
                continue;
            if (!reached[next]) {
                reached[next] = 1;
                queue[tail++] = next;
            }
        }
    }

    for (s = 0; s < N_STATES; s++) {
        if (!reached[s]) {
            fprintf(out, "state cannot be reached from %s: %s\n",
                    state_name(start), state_name((state_t)s));
            problems++;
        }
    }
    return problems;
}

/* Problem 4: finds the states with no way out: no event leads to a
   different state. Entries that were left out or point out of range do
   not count as a way out. */
static int check_dead_ends(const transition_t table[N_STATES][N_EVENTS],
                           FILE *out)
{
    int problems = 0;
    int s, e;

    for (s = 0; s < N_STATES; s++) {
        int way_out = 0;

        for (e = 0; e < N_EVENTS; e++) {
            const transition_t *t = &table[s][e];
            int next = (int)t->next;

            if (t->action == A_UNDEFINED || next < 0 || next >= N_STATES)
                continue;
            if (next != s)
                way_out = 1;
        }
        if (!way_out) {
            fprintf(out, "state has no way out: %s\n",
                    state_name((state_t)s));
            problems++;
        }
    }
    return problems;
}

int fsm_check(const transition_t table[N_STATES][N_EVENTS],
              state_t start, FILE *out)
{
    int problems = 0;

    problems += check_entries(table, out);
    problems += check_reachable(table, start, out);
    problems += check_dead_ends(table, out);
    return problems;
}
