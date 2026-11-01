#include <stdio.h>
#include "lock.h"

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

/* The 16 expected entries, from the paper table. */
static const struct {
    state_t s;
    event_t e;
    state_t next;
    action_t action;
} expected[] = {
    { LOCKED, E_RIGHT,   GOT1,   A_NONE   },
    { LOCKED, E_WRONG,   LOCKED, A_BEEP   },
    { LOCKED, E_CLOSE,   LOCKED, A_NONE   },
    { LOCKED, E_TIMEOUT, LOCKED, A_NONE   },
    { GOT1,   E_RIGHT,   GOT2,   A_NONE   },
    { GOT1,   E_WRONG,   LOCKED, A_BEEP   },
    { GOT1,   E_CLOSE,   GOT1,   A_NONE   },
    { GOT1,   E_TIMEOUT, LOCKED, A_NONE   },
    { GOT2,   E_RIGHT,   OPEN,   A_UNLOCK },
    { GOT2,   E_WRONG,   LOCKED, A_BEEP   },
    { GOT2,   E_CLOSE,   GOT2,   A_NONE   },
    { GOT2,   E_TIMEOUT, LOCKED, A_NONE   },
    { OPEN,   E_RIGHT,   OPEN,   A_NONE   },
    { OPEN,   E_WRONG,   OPEN,   A_NONE   },
    { OPEN,   E_CLOSE,   LOCKED, A_LOCK   },
    { OPEN,   E_TIMEOUT, LOCKED, A_LOCK   },
};

/* Checks every table entry through lock_step. */
static void test_table(void)
{
    int i;
    int n = (int)(sizeof expected / sizeof expected[0]);

    for (i = 0; i < n; i++) {
        char name[80];
        action_t a = A_UNDEFINED;
        state_t next = lock_step(expected[i].s, expected[i].e, &a);

        snprintf(name, sizeof name, "table: %s + %s -> %s %s",
                 state_name(expected[i].s), event_name(expected[i].e),
                 state_name(expected[i].next),
                 action_name(expected[i].action));
        check(name, next == expected[i].next && a == expected[i].action);
    }
}

/* Checks lock_event: right and wrong digits in each state, while open,
   the characters c and t, and other characters. */
static void test_event(void)
{
    check("event: LOCKED, 3 is RIGHT", lock_event(LOCKED, '3') == E_RIGHT);
    check("event: LOCKED, 1 is WRONG", lock_event(LOCKED, '1') == E_WRONG);
    check("event: GOT1, 1 is RIGHT",   lock_event(GOT1, '1') == E_RIGHT);
    check("event: GOT1, 3 is WRONG",   lock_event(GOT1, '3') == E_WRONG);
    check("event: GOT2, 4 is RIGHT",   lock_event(GOT2, '4') == E_RIGHT);
    check("event: GOT2, 3 is WRONG",   lock_event(GOT2, '3') == E_WRONG);
    check("event: OPEN, 3 is WRONG",   lock_event(OPEN, '3') == E_WRONG);
    check("event: OPEN, 1 is WRONG",   lock_event(OPEN, '1') == E_WRONG);
    check("event: OPEN, 4 is WRONG",   lock_event(OPEN, '4') == E_WRONG);
    check("event: OPEN, 0 is WRONG",   lock_event(OPEN, '0') == E_WRONG);
    check("event: c is CLOSE in LOCKED", lock_event(LOCKED, 'c') == E_CLOSE);
    check("event: c is CLOSE in OPEN",   lock_event(OPEN, 'c') == E_CLOSE);
    check("event: t is TIMEOUT in GOT1", lock_event(GOT1, 't') == E_TIMEOUT);
    check("event: t is TIMEOUT in OPEN", lock_event(OPEN, 't') == E_TIMEOUT);
    check("event: x is -1",       lock_event(LOCKED, 'x') == -1);
    check("event: space is -1",   lock_event(GOT2, ' ') == -1);
    check("event: newline is -1", lock_event(OPEN, '\n') == -1);
}

/* Starts in LOCKED, feeds the characters, returns the final state. */
static state_t run(const char *seq)
{
    state_t s = LOCKED;

    for (; *seq != '\0'; seq++) {
        int ev = lock_event(s, *seq);
        action_t a;

        if (ev < 0)
            continue;
        s = lock_step(s, (event_t)ev, &a);
    }
    return s;
}

/* The four sequences from the assignment. */
static void test_sequences(void)
{
    check("sequence: 3 1 4 opens",           run("314") == OPEN);
    check("sequence: 3 3 1 4 does not open", run("3314") != OPEN);
    check("sequence: 3 1 t 4 does not open", run("31t4") != OPEN);
    check("sequence: 3 1 4 c locks again",   run("314c") == LOCKED);
}

/* The names, including out of range. */
static void test_names(void)
{
    check("name: state_name(GOT1)",         state_name(GOT1)[0] == 'G');
    check("name: state_name out of range",  state_name(N_STATES)[0] == '?');
    check("name: event_name out of range",  event_name(N_EVENTS)[0] == '?');
    check("name: action_name out of range", action_name(N_ACTIONS)[0] == '?');
}

int main(void)
{
    test_table();
    test_event();
    test_sequences();
    test_names();

    printf("\n%d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
