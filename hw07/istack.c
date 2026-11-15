#include <stdlib.h>
#include "istack.h"

struct istack {
    int *data;        /* the values, in one array on the heap */
    size_t count;     /* how many values it holds */
    size_t capacity;  /* how many fit right now */
};

istack_t *istack_create(void)
{
    istack_t *s = malloc(sizeof *s);

    if (s == NULL)
        return NULL;
    s->data = malloc(ISTACK_START * sizeof *s->data);
    if (s->data == NULL) {
        free(s);                    /* give back the first block: no leak */
        return NULL;
    }
    s->count = 0;
    s->capacity = ISTACK_START;
    return s;
}

void istack_destroy(istack_t *s)
{
    if (s == NULL)
        return;
    free(s->data);                  /* the array first, then the struct */
    free(s);
}

int istack_push(istack_t *s, int v)
{
    int *bigger;

    if (s == NULL)
        return -1;
    if (s->count == s->capacity) {              /* full: double the room */
        bigger = realloc(s->data, 2 * s->capacity * sizeof *s->data);
        if (bigger == NULL)
            return -1;                          /* s->data is still good */
        s->data = bigger;
        s->capacity = 2 * s->capacity;
    }
    s->data[s->count] = v;                      /* the new top */
    s->count++;
    return 0;
}

int istack_pop(istack_t *s, int *v)
{
    if (s == NULL || s->count == 0)
        return -1;                      /* *v is not touched */
    if (v != NULL)
        *v = s->data[s->count - 1];     /* the top value */
    s->count--;                         /* the slot is free again */
    return 0;
}

int istack_peek(const istack_t *s, int *v)
{
    if (s == NULL || s->count == 0)
        return -1;
    if (v != NULL)
        *v = s->data[s->count - 1];
    return 0;
}

size_t istack_count(const istack_t *s)
{
    return s == NULL ? 0 : s->count;
}

bool istack_is_empty(const istack_t *s)
{
    return s == NULL || s->count == 0;
}

size_t istack_capacity(const istack_t *s)
{
    return s == NULL ? 0 : s->capacity;
}

int *istack_to_array(const istack_t *s, size_t *n)
{
    int *copy;
    size_t i;

    if (n == NULL)
        return NULL;
    *n = 0;                             /* the answer if anything fails */
    if (s == NULL || s->count == 0)
        return NULL;
    copy = malloc(s->count * sizeof *copy);
    if (copy == NULL)
        return NULL;
    for (i = 0; i < s->count; i++)      /* bottom first */
        copy[i] = s->data[i];
    *n = s->count;
    return copy;
}
