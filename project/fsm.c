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

#include "bits.h"

void fsm_init(fsm_t *f)
{
    f->state = CALM;
    f->below = 0;
    f->buzzer = 0;
}

/* Takes one event through the table and carries out its action. */
static void apply(fsm_t *f, fsm_event_t e)
{
    fsm_action_t action;

    f->state = fsm_step(f->state, e, &action);
    if (action == A_BUZZER_ON)
        f->buzzer = 1;
    else if (action == A_BUZZER_OFF)
        f->buzzer = 0;
}

void fsm_tick(fsm_t *f, float temp_c, float rh_pct, int ack_pressed)
{
    int level = fsm_level(temp_c, rh_pct);
    fsm_event_t e;

    if (ack_pressed)                    /* ACK first, see fsm.h */
        apply(f, E_ACK);

    if (level < (int)f->state) {        /* the reading is below this level */
        f->below++;
        if (f->below >= FSM_HOLD) {
            e = E_LOWER;
            f->below = 0;               /* each level needs its own count */
        } else {
            e = (fsm_event_t)level;     /* E_L0..E_L3 are numbered 0..3 */
        }
    } else {                            /* at or above: the count starts over */
        f->below = 0;
        e = (fsm_event_t)level;
    }
    apply(f, e);
}

uint32_t fsm_leds(const fsm_t *f)
{
    uint32_t leds = 0;

    if (f->state == WATCH)
        leds |= 0x1;
    if (f->state == WARNING)
        leds |= 0x2;
    if (f->state == SEVERE)
        leds |= 0x4;
    if (f->buzzer)
        leds |= 0x8;
    return leds;
}

uint32_t fsm_status(const fsm_t *f)
{
    uint32_t word = 0;

    word = set_field(word, 0, 2, (uint32_t)f->state);
    word = set_field(word, 2, 4, fsm_leds(f));
    return word;
}
