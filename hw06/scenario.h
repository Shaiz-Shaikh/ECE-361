#ifndef SCENARIO_H
#define SCENARIO_H

#include <stdint.h>
#include <time.h>

/*
 * scenario: a scenario file, loaded into a fixed array (no malloc).
 *
 * Format: '#' starts a comment (alone or after data). The first line that is
 * not a comment or blank is "start <unix timestamp>". Each line after it is
 * "tick temp rh switches", the switches in hexadecimal (0x1), ticks strictly
 * increasing. Between lines the values hold.
 */

#define SCENARIO_MAX 1000       /* most tick lines one scenario can hold */

typedef struct {
    int      tick;              /* seconds since start */
    float    temp_c;
    float    rh_pct;
    uint32_t switches;          /* the switch register */
} scen_line_t;

typedef struct {
    time_t      start;          /* the start time, from the start line */
    int         n;              /* how many tick lines are used */
    scen_line_t lines[SCENARIO_MAX];
} scenario_t;

/* scenario_load
 * Takes:   the path of a scenario file and where to put the result.
 * Does:    reads the file; line numbers count every line of the file,
 *          including comments and blank lines.
 * Returns: 0 on success; the number of the first wrong line (a missing
 *          start line, a missing or extra field, ticks that do not
 *          increase, a line longer than 255 characters, or more than
 *          SCENARIO_MAX lines); -1 if the file cannot be opened or a
 *          pointer is NULL. On failure *s may be partly filled. */
int scenario_load(const char *path, scenario_t *s);

/* scenario_at
 * Takes:   a loaded scenario and a tick.
 * Returns: the line in force at that tick: the last line whose tick is not
 *          above it, so the values hold between lines; NULL if the
 *          scenario is empty or the tick is before the first line. */
const scen_line_t *scenario_at(const scenario_t *s, int tick);

/* scenario_last_tick
 * Returns: the tick of the last line, or -1 if the scenario is empty. */
int scenario_last_tick(const scenario_t *s);

#endif
