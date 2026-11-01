#include "lock.h"

/* One entry for every (state, event) pair, written with designated
   initializers: [STATE] = { [EVENT] = { next state, action } }. */
const transition_t lock_table[N_STATES][N_EVENTS] = {
    [LOCKED] = {
        [E_RIGHT]   = { GOT1,   A_NONE },
        [E_WRONG]   = { LOCKED, A_BEEP },
        [E_CLOSE]   = { LOCKED, A_NONE },
        [E_TIMEOUT] = { LOCKED, A_NONE },
    },
    [GOT1] = {
        [E_RIGHT]   = { GOT2,   A_NONE },
        [E_WRONG]   = { LOCKED, A_BEEP },
        [E_CLOSE]   = { GOT1,   A_NONE },
        [E_TIMEOUT] = { LOCKED, A_NONE },
    },
    [GOT2] = {
        [E_RIGHT]   = { OPEN,   A_UNLOCK },
        [E_WRONG]   = { LOCKED, A_BEEP },
        [E_CLOSE]   = { GOT2,   A_NONE },
        [E_TIMEOUT] = { LOCKED, A_NONE },
    },
    [OPEN] = {
        [E_RIGHT]   = { OPEN,   A_NONE },
        [E_WRONG]   = { OPEN,   A_NONE },
        [E_CLOSE]   = { LOCKED, A_LOCK },
        [E_TIMEOUT] = { LOCKED, A_LOCK },
    },
};

/* One table lookup. No if or switch on the state. */
state_t lock_step(state_t s, event_t e, action_t *action)
{
    const transition_t *t = &lock_table[s][e];

    *action = t->action;
    return t->next;
}

/* Names, indexed by the enum values. */
static const char *const state_names[N_STATES] = {
    [LOCKED] = "LOCKED", [GOT1] = "GOT1", [GOT2] = "GOT2", [OPEN] = "OPEN"
};
static const char *const event_names[N_EVENTS] = {
    [E_RIGHT] = "RIGHT", [E_WRONG] = "WRONG",
    [E_CLOSE] = "CLOSE", [E_TIMEOUT] = "TIMEOUT"
};
static const char *const action_names[N_ACTIONS] = {
    [A_UNDEFINED] = "UNDEFINED", [A_NONE] = "NONE", [A_UNLOCK] = "UNLOCK",
    [A_LOCK] = "LOCK", [A_BEEP] = "BEEP"
};

const char *state_name(state_t s)
{
    if ((int)s < 0 || (int)s >= N_STATES)
        return "?";
    return state_names[s];
}

const char *event_name(event_t e)
{
    if ((int)e < 0 || (int)e >= N_EVENTS)
        return "?";
    return event_names[e];
}

const char *action_name(action_t a)
{
    if ((int)a < 0 || (int)a >= N_ACTIONS)
        return "?";
    return action_names[a];
}

/* The code. In state LOCKED, GOT1, GOT2 (numbered 0, 1, 2), the next
   right digit is CODE[s]. */
static const char CODE[] = "314";

int lock_event(state_t s, char c)
{
    if (c == 'c')
        return E_CLOSE;
    if (c == 't')
        return E_TIMEOUT;
    if (c >= '0' && c <= '9') {
        if ((int)s >= 0 && (int)s < 3 && c == CODE[s])
            return E_RIGHT;
        return E_WRONG;
    }
    return -1;
}
