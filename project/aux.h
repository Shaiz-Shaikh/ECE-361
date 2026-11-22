#ifndef AUX_H
#define AUX_H

#include <stdbool.h>
#include <stddef.h>
#include "sensor.h"

/*
 * aux: a circular buffer that holds the last 30 readings (S3). When it is
 * full, a new reading overwrites the oldest. The type is opaque: only aux.c
 * knows what is inside.
 */

#define AUX_N 30                    /* how many readings it holds */

typedef struct aux aux_t;           /* opaque */

/* aux_create
 * Takes:   nothing.
 * Returns: a new, empty buffer, or NULL if memory runs out. The caller owns
 *          it and must call aux_destroy on it. */
aux_t *aux_create(void);

/* aux_destroy
 * Takes:   a buffer, or NULL.
 * Does:    frees it. NULL does nothing. The buffer must not be used
 *          afterwards. */
void aux_destroy(aux_t *a);

/* aux_add
 * Takes:   a buffer a and a reading r.
 * Does:    stores a copy of *r as the newest entry. If the buffer already
 *          holds AUX_N readings, the oldest one is overwritten.
 * Returns: 0 on success. Returns 1 and changes nothing if a or r is NULL. */
int aux_add(aux_t *a, const reading_t *r);

/* aux_get
 * Takes:   a buffer a, a position i, and out, where to store a copy.
 *          i counts from the newest: 0 is the newest entry, 1 the one
 *          before it, and so on.
 * Returns: 0 on success. Returns 1 and changes nothing if a or out is NULL,
 *          or if i is not less than aux_count(a). */
int aux_get(const aux_t *a, size_t i, reading_t *out);

/* aux_count
 * Returns: how many readings it holds, from 0 up to AUX_N. NULL gives 0. */
size_t aux_count(const aux_t *a);

/* aux_is_full
 * Returns: true if it holds AUX_N readings. NULL gives false. */
bool aux_is_full(const aux_t *a);

#endif
