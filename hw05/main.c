#include <stdio.h>
#include "lock.h"

int main(void)
{
    state_t state = LOCKED;
    int c;

    while ((c = getchar()) != EOF) {
        int ev = lock_event(state, (char)c);
        action_t action;
        state_t next;

        if (ev < 0)
            continue;               /* spaces, newlines: not an event */
        next = lock_step(state, (event_t)ev, &action);
        printf("%s %c %s -> %s %s\n", state_name(state), c,
               event_name((event_t)ev), state_name(next),
               action_name(action));
        state = next;
    }
    printf("final: %s\n", state_name(state));
    return 0;
}
