#include <stddef.h>
#include <string.h>
#include "tally.h"

/* One name and how often it was added. */
typedef struct {
    char name[TALLY_LEN + 1];       /* +1 for the '\0' */
    int  count;
} entry_t;

/* The inside of a tally. Only this file knows it. */
struct tally {
    entry_t entries[TALLY_NAMES];   /* kept sorted by name */
    int     n;                      /* how many entries are used */
    int     in_use;                 /* 1 = handed out, 0 = free */
};

/* The pool: TALLY_MAX tallies that exist for the whole program. */
static struct tally pool[TALLY_MAX];

/* Is name usable? Not NULL, not empty, not longer than TALLY_LEN. */
static int name_ok(const char *name)
{
    size_t len;

    if (name == NULL) {
        return 0;
    }
    len = strlen(name);
    return len >= 1 && len <= TALLY_LEN;
}

/* Finds where name is, or where it belongs. Returns the index of the first
 * entry that is not smaller than name, or t->n if every entry is smaller.
 * Sets *found to 1 if that entry is equal to name, otherwise 0. */
static int find_pos(const struct tally *t, const char *name, int *found)
{
    int i = 0;

    *found = 0;
    while (i < t->n) {
        int cmp = strcmp(t->entries[i].name, name);
        if (cmp == 0) {
            *found = 1;
            return i;
        }
        if (cmp > 0) {
            return i;
        }
        i++;
    }
    return t->n;
}

tally_t *tally_create(void)
{
    int i = 0;

    while (i < TALLY_MAX) {
        if (!pool[i].in_use) {
            pool[i].in_use = 1;
            pool[i].n = 0;
            return &pool[i];
        }
        i++;
    }
    return NULL;
}

void tally_destroy(tally_t *t)
{
    if (t == NULL) {
        return;
    }
    t->in_use = 0;
    t->n = 0;
}

int tally_count(const tally_t *t, const char *name)
{
    int pos;
    int found;

    if (t == NULL || name == NULL) {
        return 0;
    }
    pos = find_pos(t, name, &found);
    if (!found) {
        return 0;
    }
    return t->entries[pos].count;
}

int tally_size(const tally_t *t)
{
    if (t == NULL) {
        return 0;
    }
    return t->n;
}

int tally_add(tally_t *t, const char *name)
{
    int pos;
    int found;
    int i;
    size_t len;

    if (t == NULL || !name_ok(name)) {
        return -1;
    }

    pos = find_pos(t, name, &found);
    if (found) {
        t->entries[pos].count++;
        return t->entries[pos].count;
    }

    if (t->n >= TALLY_NAMES) {
        return -1;
    }

    /* Make room: move the later entries up by one, starting from the
     * last one, so no entry is overwritten before it has been moved. */
    for (i = t->n; i > pos; i--) {
        t->entries[i] = t->entries[i - 1];
    }

    len = strlen(name);
    memcpy(t->entries[pos].name, name, len + 1);   /* len <= TALLY_LEN, checked */
    t->entries[pos].count = 1;
    t->n++;
    return 1;
}

void tally_print(const tally_t *t, FILE *out)
{
    int i;

    if (t == NULL || out == NULL) {
        return;
    }
    for (i = 0; i < t->n; i++) {
        fprintf(out, "%s %d\n", t->entries[i].name, t->entries[i].count);
    }
}
