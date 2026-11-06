#include "fsm.h"

/* One entry for every (state, event) pair: [STATE] = { [EVENT] = { next, action } }. */
const fsm_transition_t fsm_table[N_FSM_STATES][N_FSM_EVENTS] = {
    [CALM] = {
        [E_L0]    = { CALM,    A_NONE },
        [E_L1]    = { WATCH,   A_NONE },
        [E_L2]    = { WARNING, A_BUZZER_ON },
        [E_L3]    = { SEVERE,  A_BUZZER_ON },
        [E_LOWER] = { CALM,    A_NONE },
        [E_ACK]   = { CALM,    A_NONE },
    },
    [WATCH] = {
        [E_L0]    = { WATCH,   A_NONE },
        [E_L1]    = { WATCH,   A_NONE },
        [E_L2]    = { WARNING, A_BUZZER_ON },
        [E_L3]    = { SEVERE,  A_BUZZER_ON },
        [E_LOWER] = { CALM,    A_NONE },
        [E_ACK]   = { WATCH,   A_NONE },
    },
    [WARNING] = {
        [E_L0]    = { WARNING, A_NONE },
        [E_L1]    = { WARNING, A_NONE },
        [E_L2]    = { WARNING, A_NONE },
        [E_L3]    = { SEVERE,  A_BUZZER_ON },
        [E_LOWER] = { WATCH,   A_NONE },
        [E_ACK]   = { WARNING, A_BUZZER_OFF },
    },
    [SEVERE] = {
        [E_L0]    = { SEVERE,  A_NONE },
        [E_L1]    = { SEVERE,  A_NONE },
        [E_L2]    = { SEVERE,  A_NONE },
        [E_L3]    = { SEVERE,  A_NONE },
        [E_LOWER] = { WARNING, A_NONE },
        [E_ACK]   = { SEVERE,  A_BUZZER_OFF },
    },
};

/* Rule 1 of the sheet: the level a single reading says. Checked from the
   highest level down, so the first match wins. */
int fsm_level(float temp_c, float rh_pct)
{
    if (temp_c >= 40.0f)
        return 3;
    if (temp_c >= 35.0f || (temp_c >= 32.0f && rh_pct >= 70.0f))
        return 2;
    if (temp_c >= 30.0f)
        return 1;
    return 0;
}

/* One table lookup. No if or switch on the state. */
fsm_state_t fsm_step(fsm_state_t s, fsm_event_t e, fsm_action_t *action)
{
    const fsm_transition_t *t = &fsm_table[s][e];

    *action = t->action;
    return t->next;
}

/* Names, indexed by the enum values. */
static const char *const state_names[N_FSM_STATES] = {
    [CALM] = "CALM", [WATCH] = "WATCH", [WARNING] = "WARNING",
    [SEVERE] = "SEVERE"
};
static const char *const event_names[N_FSM_EVENTS] = {
    [E_L0] = "L0", [E_L1] = "L1", [E_L2] = "L2", [E_L3] = "L3",
    [E_LOWER] = "LOWER", [E_ACK] = "ACK"
};
static const char *const action_names[N_FSM_ACTIONS] = {
    [A_UNDEFINED] = "UNDEFINED", [A_NONE] = "NONE",
    [A_BUZZER_ON] = "BUZZER_ON", [A_BUZZER_OFF] = "BUZZER_OFF"
};

const char *fsm_state_name(fsm_state_t s)
{
    if ((int)s < 0 || (int)s >= N_FSM_STATES)
        return "?";
    return state_names[s];
}

const char *fsm_event_name(fsm_event_t e)
{
    if ((int)e < 0 || (int)e >= N_FSM_EVENTS)
        return "?";
    return event_names[e];
}

const char *fsm_action_name(fsm_action_t a)
{
    if ((int)a < 0 || (int)a >= N_FSM_ACTIONS)
        return "?";
    return action_names[a];
}
