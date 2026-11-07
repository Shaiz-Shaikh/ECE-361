#ifndef FSM_H
#define FSM_H

/*
 * fsm: the state machine of the weather station. The behavior is one table:
 * fsm_table[state][event] gives the next state and the action. Levels:
 * CALM 0, WATCH 1, WARNING 2, SEVERE 3.
 */

typedef enum { CALM, WATCH, WARNING, SEVERE, N_FSM_STATES } fsm_state_t;

/* E_L0..E_L3: a reading at that level. E_LOWER: 30 readings in a row below
   the current level. E_ACK: the ACK switch was pressed. */
typedef enum {
    E_L0, E_L1, E_L2, E_L3, E_LOWER, E_ACK, N_FSM_EVENTS
} fsm_event_t;

typedef enum {
    A_UNDEFINED, A_NONE, A_BUZZER_ON, A_BUZZER_OFF, N_FSM_ACTIONS
} fsm_action_t;

typedef struct { fsm_state_t next; fsm_action_t action; } fsm_transition_t;

/* The transition table: one entry for every (state, event) pair.
   Read-only; defined in fsm.c. */
extern const fsm_transition_t fsm_table[N_FSM_STATES][N_FSM_EVENTS];

/* fsm_level
 * Takes:   a temperature in degrees C and a humidity in %RH.
 * Returns: the level of that reading: 3 at 40.0 C or more; 2 at 35.0 C or
 *          more, or at 32.0 C or more with 70 %RH or more; 1 at 30.0 C or
 *          more; otherwise 0. */
int fsm_level(float temp_c, float rh_pct);

/* fsm_step
 * Takes:   the current state s, an event e, and where to store the action.
 * Returns: the next state, found with one table lookup.
 * Assumes: s and e are in range and action is not NULL. */
fsm_state_t fsm_step(fsm_state_t s, fsm_event_t e, fsm_action_t *action);

/* fsm_state_name, fsm_event_name, fsm_action_name
 * Take:    a state, an event, or an action value.
 * Return:  its name as a string, or "?" if the value is out of range. */
const char *fsm_state_name(fsm_state_t s);
const char *fsm_event_name(fsm_event_t e);
const char *fsm_action_name(fsm_action_t a);


#include <stdint.h>

#define FSM_HOLD 30     /* readings in a row below a level before it is lowered */

/* fsm_t: a state machine at work: the state, how many readings in a row have
   been below the current level, and whether the buzzer is sounding. */
typedef struct {
    fsm_state_t state;
    int         below;
    int         buzzer;
} fsm_t;

/* fsm_init
 * Takes:   a state machine. Does: CALM, no count, buzzer off. */
void fsm_init(fsm_t *f);

/* fsm_tick
 * Takes:   the state machine, one reading (temperature, humidity), and
 *          whether ACK was just pressed (nonzero) in this tick.
 * Does:    ACK first (so an alarm raised in this tick still sounds); then
 *          the reading's event: its level E_L0..E_L3, or E_LOWER after
 *          FSM_HOLD readings in a row below the state's level. Actions turn
 *          the buzzer on or off.
 * Assumes: f is not NULL. */
void fsm_tick(fsm_t *f, float temp_c, float rh_pct, int ack_pressed);

/* fsm_leds
 * Returns: the LED register value for this state: bit 0 WATCH_LAMP (state
 *          WATCH), bit 1 WARNING_LAMP, bit 2 SEVERE_LAMP, bit 3 BUZZER. */
uint32_t fsm_leds(const fsm_t *f);

/* fsm_status
 * Returns: the status word: bits 1..0 the state, bits 5..2 the LED bits
 *          (WATCH_LAMP, WARNING_LAMP, SEVERE_LAMP, BUZZER), packed with
 *          set_field. */
uint32_t fsm_status(const fsm_t *f);

#endif
