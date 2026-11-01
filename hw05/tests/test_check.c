#include <stdio.h>
#include <string.h>
#include "check.h"

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

/* Fills copy with the real lock_table, so a test can break the copy. */
static void copy_table(transition_t copy[N_STATES][N_EVENTS])
{
    memcpy(copy, lock_table, sizeof(transition_t) * N_STATES * N_EVENTS);
}

/* Runs the checker on a table that may have been changed, from LOCKED.
   The cast only adds "const": fsm_check promises not to change the table. */
static int run_checker(transition_t t[N_STATES][N_EVENTS])
{
    return fsm_check((const transition_t (*)[N_EVENTS])t, LOCKED, stdout);
}

int main(void)
{
    transition_t t[N_STATES][N_EVENTS];
    int n;

    printf("--- the real lock_table\n");
    n = fsm_check(lock_table, LOCKED, stdout);
    check("real table: 0 problems", n == 0);

    printf("--- broken copy 1: GOT2 + RIGHT left out\n");
    copy_table(t);
    t[GOT2][E_RIGHT].action = A_UNDEFINED;
    t[GOT2][E_RIGHT].next = LOCKED;
    n = run_checker(t);
    check("broken 1: left out entry and OPEN unreachable (2)", n == 2);

    printf("--- broken copy 2: GOT1 + RIGHT goes to state 7\n");
    copy_table(t);
    t[GOT1][E_RIGHT].next = (state_t)7;
    n = run_checker(t);
    check("broken 2: bad next state, GOT2 and OPEN unreachable (3)", n == 3);

    printf("--- broken copy 3: OPEN never locks again\n");
    copy_table(t);
    t[OPEN][E_CLOSE].next = OPEN;
    t[OPEN][E_CLOSE].action = A_NONE;
    t[OPEN][E_TIMEOUT].next = OPEN;
    t[OPEN][E_TIMEOUT].action = A_NONE;
    n = run_checker(t);
    check("broken 3: OPEN has no way out (1)", n == 1);

    printf("\n%d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
