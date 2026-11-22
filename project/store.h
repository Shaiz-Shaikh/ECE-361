#ifndef STORE_H
#define STORE_H

#include <stdint.h>
#include "sensor.h"

/*
 * store: the record of everything the station has measured, ordered by
 * temperature in 0.5 C steps (key K2). A binary search tree with one node per
 * distinct key; the node holds every record with that key, in arrival order,
 * so many equal readings do not make it taller. Callers only use this header.
 */

/* record_t: the reading, the state at that moment (for now a plain int,
   the fsm state), and the status word (state, lamps and buzzer packed). */
typedef struct {
    reading_t reading;
    int       state;
    uint32_t  status;
} record_t;

typedef struct store store_t;           /* opaque */

/* record_key
 * Takes:   a record.
 * Returns: its key: the temperature quantized to 0.5 C, floor(temp * 2),
 *          so 21.2 and 21.4 C both give 42.
 * Assumes: r is not NULL. */
int record_key(const record_t *r);

/* record_compare
 * Takes:   two records.
 * Returns: negative if a's key is below b's, 0 if the keys are equal,
 *          positive if a's key is above b's. Only the key counts.
 * Assumes: a and b are not NULL. */
int record_compare(const record_t *a, const record_t *b);

/* store_create
 * Returns: a new, empty store; NULL if memory ran out. */
store_t *store_create(void);

/* store_destroy
 * Takes:   a store from store_create. Frees the store and every record in it.
 *          NULL does nothing. */
void store_destroy(store_t *s);

/* store_insert
 * Takes:   a store and a record. The record is copied.
 * Does:    keeps the store in key order. A record whose key already exists
 *          goes after the ones with that key (arrival order); none is lost.
 * Returns: 0 on success; -1 and changes nothing if s or r is NULL or
 *          memory ran out. */
int store_insert(store_t *s, const record_t *r);

/* store_find
 * Takes:   a store and a key (see record_key).
 * Returns: a pointer to the first record with that key (the earliest
 *          arrival), or NULL if there is none or s is NULL. The record
 *          belongs to the store: do not change or free it. */
const record_t *store_find(const store_t *s, int key);

/* store_walk
 * Takes:   a store, a callback, and a pointer arg passed to the callback.
 * Does:    calls visit(record, arg) for every record in key order (equal
 *          keys in arrival order). Does nothing if s or visit is NULL.
 * Assumes: the callback does not insert into or destroy the store. */
void store_walk(const store_t *s,
                void (*visit)(const record_t *r, void *arg), void *arg);

/* store_count
 * Returns: how many records the store holds; 0 for NULL. */
int store_count(const store_t *s);

/* store_height
 * Returns: the height of the tree: the number of keys on the longest path
 *          from the root down. Records with the same key share one node, so
 *          they do not add height. 0 for NULL or empty. */
int store_height(const store_t *s);

#endif
