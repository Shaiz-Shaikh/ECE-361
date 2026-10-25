#ifndef TALLY_H
#define TALLY_H

#include <stdio.h>

#define TALLY_MAX        4      /* tallies that can exist at the same time */
#define TALLY_NAMES     16      /* different names one tally can hold */
#define TALLY_LEN       15      /* longest name, without the '\0' */

typedef struct tally tally_t;            /* opaque */

/* tally_create
 * Takes:   nothing.
 * Returns: a new, empty tally, or NULL if TALLY_MAX tallies are already
 *          in use.
 * Assumes: nothing. */
tally_t *tally_create(void);

/* tally_destroy
 * Takes:   t, a tally from tally_create.
 * Does:    gives the tally back, so tally_create can hand it out again.
 *          NULL does nothing.
 * Returns: nothing.
 * Assumes: t was not already destroyed. */
void tally_destroy(tally_t *t);

/* tally_add
 * Takes:   t, a tally; name, a string.
 * Does:    counts one more occurrence of name. A new name is inserted so
 *          the names stay sorted.
 * Returns: the new count of name. Returns -1 and changes nothing if t or
 *          name is NULL, name is empty or longer than TALLY_LEN, or name
 *          is new and the tally is full.
 * Assumes: name is a '\0'-terminated string. */
int tally_add(tally_t *t, const char *name);

/* tally_count
 * Takes:   t, a tally; name, a string.
 * Returns: how often name was added: 0 if never, or if t or name is NULL.
 * Assumes: name is a '\0'-terminated string. */
int tally_count(const tally_t *t, const char *name);

/* tally_size
 * Takes:   t, a tally.
 * Returns: how many different names the tally holds; 0 if t is NULL.
 * Assumes: nothing. */
int tally_size(const tally_t *t);

/* tally_print
 * Takes:   t, a tally; out, where to write.
 * Does:    writes one line per name, "name count", in alphabetical
 *          (strcmp) order. Does nothing if t or out is NULL.
 * Returns: nothing.
 * Assumes: out is open for writing. */
void tally_print(const tally_t *t, FILE *out);

#endif
