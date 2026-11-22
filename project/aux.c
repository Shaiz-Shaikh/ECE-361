#include <stdlib.h>
#include "aux.h"

struct aux {
    reading_t data[AUX_N];  /* the slots */
    size_t head;            /* the slot where the next reading goes */
    size_t count;           /* how many slots hold a reading */
};

aux_t *aux_create(void)
{
    aux_t *a = malloc(sizeof *a);

    if (a == NULL)
        return NULL;
    a->head = 0;
    a->count = 0;
    return a;
}

void aux_destroy(aux_t *a)
{
    free(a);                /* one block, so one free; NULL is fine */
}

int aux_add(aux_t *a, const reading_t *r)
{
    if (a == NULL || r == NULL)
        return 1;
    a->data[a->head] = *r;              /* overwrites the oldest when full */
    a->head = (a->head + 1) % AUX_N;    /* wrap around after the last slot */
    if (a->count < AUX_N)
        a->count++;
    return 0;
}

int aux_get(const aux_t *a, size_t i, reading_t *out)
{
    if (a == NULL || out == NULL || i >= a->count)
        return 1;
    /* the newest is just before head; AUX_N is added first so the
     * unsigned subtraction never goes below 0 */
    *out = a->data[(a->head + AUX_N - 1 - i) % AUX_N];
    return 0;
}

size_t aux_count(const aux_t *a)
{
    return a == NULL ? 0 : a->count;
}

bool aux_is_full(const aux_t *a)
{
    return a != NULL && a->count == AUX_N;
}
