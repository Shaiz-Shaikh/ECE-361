#ifndef LOCK_H
#define LOCK_H

/* The combination lock is 3-1-4. Its behavior is one table:
   lock_table[state][event] gives the next state and the action. */

typedef enum { LOCKED, GOT1, GOT2, OPEN, N_STATES } state_t;
typedef enum { E_RIGHT, E_WRONG, E_CLOSE, E_TIMEOUT, N_EVENTS } event_t;
typedef enum { A_UNDEFINED, A_NONE, A_UNLOCK, A_LOCK, A_BEEP, N_ACTIONS } action_t;
typedef struct { state_t next; action_t action; } transition_t;

/* The transition table: one entry for every (state, event) pair.
   Read-only; defined in lock.c. */
extern const transition_t lock_table[N_STATES][N_EVENTS];

/* lock_step
   Takes:   the current state s, an event e, and a pointer to store the action.
   Returns: the next state, found with one table lookup.
   Assumes: s and e are in range (below N_STATES and N_EVENTS) and
            action is not NULL. */
state_t lock_step(state_t s, event_t e, action_t *action);

/* lock_event
   Takes:   the current state s and one input character c.
   Returns: E_RIGHT if c is the next digit of the code 3-1-4 for state s,
            E_WRONG for any other digit (including every digit while OPEN),
            E_CLOSE for 'c' (the door closing), E_TIMEOUT for 't' (the timer
            running out), and -1 for any other character.
   Assumes: s is in range. */
int lock_event(state_t s, char c);

/* state_name, event_name, action_name
   Take:    a state, an event, or an action value.
   Return:  its name as a string, like "GOT1" or "RIGHT", or "?" if the
            value is out of range.
   Assume:  nothing; the strings are read-only, do not change them. */
const char *state_name(state_t s);
const char *event_name(event_t e);
const char *action_name(action_t a);

#endif
