#include <math.h>
#include <string.h>
#include "store.h"

/* Complete the type here and nowhere else: a sorted array of records. */
struct store {
    record_t records[STORE_MAX];
    int      n;                 /* how many records are used */
    int      in_use;            /* 1 if handed out, 0 if free */
};

/* The stores live here; no malloc. */
static struct store pool[STORE_POOL];

int record_key(const record_t *r)
{
    return (int)floor((double)r->reading.temp_c * 2.0);
}

int record_compare(const record_t *a, const record_t *b)
{
    int ka = record_key(a);
    int kb = record_key(b);

    if (ka < kb)
        return -1;
    if (ka > kb)
        return 1;
    return 0;
}

store_t *store_create(void)
{
    int i;

    for (i = 0; i < STORE_POOL; i++) {
        if (!pool[i].in_use) {
            pool[i].in_use = 1;
            pool[i].n = 0;
            return &pool[i];
        }
    }
    return NULL;
}

void store_destroy(store_t *s)
{
    if (s == NULL)
        return;
    s->in_use = 0;
    s->n = 0;
}

int store_insert(store_t *s, const record_t *r)
{
    int pos;
    int i;

    if (s == NULL || r == NULL || s->n >= STORE_MAX)
        return -1;

    /* pos = the first place whose key is above r's key. Equal keys are
       skipped, so the new record goes after them (arrival order). */
    pos = 0;
    while (pos < s->n && record_compare(&s->records[pos], r) <= 0)
        pos++;

    /* Move the later records up by one, starting from the end. */
    for (i = s->n; i > pos; i--)
        s->records[i] = s->records[i - 1];

    s->records[pos] = *r;
    s->n++;
    return 0;
}

const record_t *store_find(const store_t *s, int key)
{
    int i;

    if (s == NULL)
        return NULL;
    for (i = 0; i < s->n; i++) {
        int k = record_key(&s->records[i]);

        if (k == key)
            return &s->records[i];
        if (k > key)
            break;              /* sorted: the key cannot come later */
    }
    return NULL;
}

void store_walk(const store_t *s,
                void (*visit)(const record_t *r, void *arg), void *arg)
{
    int i;

    if (s == NULL || visit == NULL)
        return;
    for (i = 0; i < s->n; i++)
        visit(&s->records[i], arg);
}

int store_count(const store_t *s)
{
    return s == NULL ? 0 : s->n;
}

int store_height(const store_t *s)
{
    return store_count(s);      /* a sorted array is a tree without branches */
}
