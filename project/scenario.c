#include <stdio.h>
#include <string.h>
#include "scenario.h"

#define LINE_BUF 256

const scen_line_t *scenario_at(const scenario_t *s, int tick)
{
    const scen_line_t *found = NULL;
    int i;

    if (s == NULL)
        return NULL;
    for (i = 0; i < s->n; i++) {
        if (s->lines[i].tick > tick)
            break;                  /* sorted: later lines are further away */
        found = &s->lines[i];       /* the latest line not above tick so far */
    }
    return found;
}

int scenario_last_tick(const scenario_t *s)
{
    if (s == NULL || s->n == 0)
        return -1;
    return s->lines[s->n - 1].tick;
}

int scenario_load(const char *path, scenario_t *s)
{
    FILE *f;
    char buf[LINE_BUF];
    int lineno = 0;
    int have_start = 0;

    if (path == NULL || s == NULL)
        return -1;
    f = fopen(path, "r");
    if (f == NULL)
        return -1;
    s->start = 0;
    s->n = 0;

    while (fgets(buf, sizeof buf, f) != NULL) {
        char *hash;
        int used = 0;

        lineno++;
        if (strchr(buf, '\n') == NULL && !feof(f)) {    /* line too long */
            fclose(f);
            return lineno;
        }
        hash = strchr(buf, '#');                        /* cut the comment */
        if (hash != NULL)
            *hash = '\0';
        if (buf[strspn(buf, " \t\r\n")] == '\0')        /* blank line */
            continue;

        if (!have_start) {
            long long ts;

            if (sscanf(buf, "start %lld %n", &ts, &used) != 1
                || buf[used] != '\0') {
                fclose(f);
                return lineno;
            }
            s->start = (time_t)ts;
            have_start = 1;
        } else {
            scen_line_t ln;
            unsigned int sw;

            if (s->n >= SCENARIO_MAX) {
                fclose(f);
                return lineno;
            }
            if (sscanf(buf, "%d %f %f %x %n", &ln.tick, &ln.temp_c,
                       &ln.rh_pct, &sw, &used) != 4 || buf[used] != '\0') {
                fclose(f);
                return lineno;
            }
            if (ln.tick < 0 || (s->n > 0 && ln.tick <= s->lines[s->n - 1].tick)) {
                fclose(f);
                return lineno;
            }
            ln.switches = sw;
            s->lines[s->n++] = ln;
        }
    }
    fclose(f);
    return have_start ? 0 : lineno + 1;                 /* no start line */
}
