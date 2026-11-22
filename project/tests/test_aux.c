#include <stdio.h>
#include "../aux.h"

static int total = 0;
static int failures = 0;

/* Prints PASS or FAIL for one check and keeps the score. */
static void check(int ok, const char *name)
{
    total++;
    if (ok) {
        printf("PASS %s\n", name);
    } else {
        printf("FAIL %s\n", name);
        failures++;
    }
}

/* A reading that is easy to recognize: reading number n. */
static reading_t mk(int n)
{
    reading_t r;

    r.t = 1000 + n;
    r.temp_c = (float)n;
    r.rh_pct = 50.0f;
    return r;
}

/* True if *r is reading number n. */
static int is_n(const reading_t *r, int n)
{
    return r->t == 1000 + n && r->temp_c == (float)n;
}

static void test_empty(void)
{
    aux_t *a = aux_create();
    reading_t out = mk(99);

    check(a != NULL, "create returns a buffer");
    check(aux_count(a) == 0, "a new buffer has count 0");
    check(!aux_is_full(a), "a new buffer is not full");
    check(aux_get(a, 0, &out) == 1 && is_n(&out, 99),
          "get on an empty buffer fails and leaves *out alone");
    aux_destroy(a);
}

static void test_fill(void)
{
    aux_t *a = aux_create();
    reading_t out = mk(0);
    int i;

    for (i = 1; i <= AUX_N; i++) {
        reading_t r = mk(i);
        aux_add(a, &r);
    }
    check(aux_count(a) == AUX_N && aux_is_full(a),
          "filled to N: the count is N and it is full");
    check(aux_get(a, 0, &out) == 0 && is_n(&out, AUX_N),
          "the newest is reading N");
    check(aux_get(a, AUX_N - 1, &out) == 0 && is_n(&out, 1),
          "the oldest is reading 1");
    check(aux_get(a, AUX_N, &out) == 1, "get beyond the count fails");
    aux_destroy(a);
}

/* One more than N: the oldest is overwritten. */
static void test_overwrite(void)
{
    aux_t *a = aux_create();
    reading_t out = mk(0);
    int i;

    for (i = 1; i <= AUX_N + 1; i++) {
        reading_t r = mk(i);
        aux_add(a, &r);
    }
    check(aux_count(a) == AUX_N, "one more than N: the count stays N");
    check(aux_get(a, 0, &out) == 0 && is_n(&out, AUX_N + 1),
          "the newest is reading N+1");
    check(aux_get(a, AUX_N - 1, &out) == 0 && is_n(&out, 2),
          "reading 1 was overwritten: the oldest is reading 2");
    aux_destroy(a);
}

/* 1.5 times around: get(i) must go from the newest back to the oldest. */
static void test_wrap_order(void)
{
    aux_t *a = aux_create();
    reading_t out = mk(0);
    int added = AUX_N + AUX_N / 2;      /* 45 readings in all */
    int i;
    int ok = 1;

    for (i = 1; i <= added; i++) {
        reading_t r = mk(i);
        aux_add(a, &r);
    }
    /* position i from the newest is reading number added - i */
    for (i = 0; i < AUX_N; i++) {
        if (aux_get(a, (size_t)i, &out) != 0 || !is_n(&out, added - i))
            ok = 0;
    }
    check(ok, "after wrapping, get(i) goes from the newest back to the oldest");
    aux_destroy(a);
}

/* The buffer keeps its own copy of what was added. */
static void test_copy(void)
{
    aux_t *a = aux_create();
    reading_t r = mk(7);
    reading_t out = mk(0);

    aux_add(a, &r);
    r.temp_c = 99.0f;                   /* change the caller's reading */
    check(aux_get(a, 0, &out) == 0 && is_n(&out, 7),
          "the buffer keeps its own copy");
    aux_destroy(a);
}

/* A NULL buffer or a NULL pointer must not crash anything. */
static void test_null(void)
{
    aux_t *a = aux_create();
    reading_t r = mk(1);
    reading_t out = mk(0);

    check(aux_add(NULL, &r) == 1, "add to NULL fails with 1");
    check(aux_add(a, NULL) == 1, "add of NULL fails with 1");
    check(aux_get(NULL, 0, &out) == 1, "get from NULL fails with 1");
    check(aux_get(a, 0, NULL) == 1, "get into NULL fails with 1");
    check(aux_count(NULL) == 0, "count of NULL is 0");
    check(!aux_is_full(NULL), "NULL is not full");
    aux_destroy(a);
    aux_destroy(NULL);
    check(1, "destroying NULL does nothing");
}

int main(void)
{
    test_empty();
    test_fill();
    test_overwrite();
    test_wrap_order();
    test_copy();
    test_null();
    printf("%d checks, %d failed\n", total, failures);
    return failures == 0 ? 0 : 1;
}
