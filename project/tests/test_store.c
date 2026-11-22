/* test_store.c: unit tests for the store (a tree of keys,
   each key holding a list of records).
   Build and run with: make test */

#include <stdio.h>
#include "store.h"

static int checks = 0;
static int failed = 0;

/* check: counts one check and prints FAIL with its name if it is false */
static void check(int ok, const char *name)
{
    checks++;
    if (!ok) {
        failed++;
        printf("FAIL: %s\n", name);
    }
}

/* mk: a record with temperature temp. n goes into the time, so two
   records with the same temperature can still be told apart. */
static record_t mk(float temp, int n)
{
    record_t r;
    r.reading.t = 1000 + n;
    r.reading.temp_c = temp;
    r.reading.rh_pct = 50.0f;
    r.state = 0;
    r.status = 0;
    return r;
}

/* test_compare: keys are 0.5 degree steps, so 21.0 to 21.4 is one
   step (key 42) and 21.5 to 21.9 is the next (key 43). */
static void test_compare(void)
{
    record_t a = mk(21.2f, 1);
    record_t b = mk(21.4f, 2);
    record_t c = mk(21.6f, 3);
    record_t d = mk(21.5f, 4);
    record_t e = mk(21.0f, 5);

    check(record_key(&a) == 42, "key of 21.2 is 42");
    check(record_key(&c) == 43, "key of 21.6 is 43");

    /* same step */
    check(record_compare(&a, &b) == 0, "compare: 21.2 vs 21.4 equal");
    check(record_compare(&b, &a) == 0, "compare: 21.4 vs 21.2 equal");

    /* neighboring steps */
    check(record_compare(&b, &c) < 0, "compare: 21.4 before 21.6");
    check(record_compare(&c, &b) > 0, "compare: 21.6 after 21.4");

    /* exactly on a step boundary */
    check(record_compare(&d, &c) == 0, "compare: 21.5 starts the step of 21.6");
    check(record_compare(&b, &d) < 0, "compare: 21.4 before 21.5");
    check(record_compare(&e, &a) == 0, "compare: 21.0 starts the step of 21.2");
}

/* collector_t: what the walk callback fills in: the time of each
   record it sees, in the order it sees them. */
typedef struct {
    long t[200];
    int  n;
} collector_t;

static void collect(const record_t *r, void *arg)
{
    collector_t *c = arg;
    if (c->n < 200)
        c->t[c->n++] = (long)r->reading.t;
}

/* test_empty: a new store has nothing in it, and can be destroyed. */
static void test_empty(void)
{
    store_t *s = store_create();
    collector_t c = { .n = 0 };

    check(s != NULL, "empty: create works");
    check(store_count(s) == 0, "empty: count is 0");
    check(store_height(s) == 0, "empty: height is 0");
    check(store_find(s, 42) == NULL, "empty: find gives NULL");
    store_walk(s, collect, &c);
    check(c.n == 0, "empty: walk visits nothing");
    store_destroy(s);
}

/* test_small: count and height after one and after two inserts. */
static void test_small(void)
{
    store_t *s = store_create();
    record_t a = mk(21.2f, 1);
    record_t b = mk(30.0f, 2);

    check(store_insert(s, &a) == 0, "small: first insert ok");
    check(store_count(s) == 1, "small: count is 1");
    check(store_height(s) == 1, "small: height is 1");
    check(store_insert(s, &b) == 0, "small: second insert ok");
    check(store_count(s) == 2, "small: count is 2");
    check(store_height(s) == 2, "small: height is 2");
    store_destroy(s);
}

/* test_find: equal keys are all kept, find gives the earliest. */
static void test_find(void)
{
    store_t *s = store_create();
    record_t a = mk(21.2f, 1);
    record_t b = mk(21.4f, 2);
    record_t c = mk(25.0f, 3);
    const record_t *p;

    store_insert(s, &a);
    store_insert(s, &b);
    store_insert(s, &c);

    p = store_find(s, 42);
    check(p != NULL && p->reading.t == 1001, "find: key 42 gives the earliest of two");
    p = store_find(s, 50);
    check(p != NULL && p->reading.t == 1003, "find: key 50 gives the 25.0 record");
    check(store_find(s, 43) == NULL, "find: a key that is not there gives NULL");
    check(store_find(NULL, 42) == NULL, "find: NULL store gives NULL");
    check(store_count(s) == 3, "find: all three records are kept");
    store_destroy(s);
}

/* test_walk_order: the A to E example: E, A, D, C, B. */
static void test_walk_order(void)
{
    store_t *s = store_create();
    record_t r[5];
    collector_t c = { .n = 0 };
    long want[5] = { 1005, 1001, 1004, 1003, 1002 };
    int same = 1;
    int i;

    r[0] = mk(21.2f, 1);   /* A: key 42 */
    r[1] = mk(22.0f, 2);   /* B: key 44 */
    r[2] = mk(21.5f, 3);   /* C: key 43 */
    r[3] = mk(21.4f, 4);   /* D: key 42, arrives after A */
    r[4] = mk(20.5f, 5);   /* E: key 41 */
    for (i = 0; i < 5; i++)
        store_insert(s, &r[i]);

    store_walk(s, collect, &c);
    for (i = 0; i < 5 && i < c.n; i++)
        if (c.t[i] != want[i])
            same = 0;
    check(c.n == 5 && same, "walk: key order, equal keys in arrival order");
    store_destroy(s);
}

/* test_many_distinct: 100 different keys, inserted from low to high.
   Sorted input makes a chain, so the height is 100 in both versions. */
static void test_many_distinct(void)
{
    store_t *s = store_create();
    int i;
    int all_ok = 1;

    for (i = 0; i < 100; i++) {
        record_t r = mk(10.0f + 0.5f * (float)i, i + 1);
        if (store_insert(s, &r) != 0)
            all_ok = 0;
    }
    check(all_ok, "distinct: all 100 inserts ok");
    check(store_count(s) == 100, "distinct: count is 100");
    check(store_height(s) == 100, "distinct: sorted input gives height 100");
    store_destroy(s);
}

/* test_many_equal: 100 readings in the same 0.5 degree step. With one
   node per key the height is 1; a sorted array or one node per record
   would give 100. This is the test the tree is for. */
static void test_many_equal(void)
{
    store_t *s = store_create();
    collector_t c = { .n = 0 };
    const record_t *p;
    int same = 1;
    int i;

    for (i = 0; i < 100; i++) {
        record_t r = mk(21.2f, i + 1);
        store_insert(s, &r);
    }
    check(store_count(s) == 100, "equal: count is 100");
    check(store_height(s) == 1, "equal: height is 1");

    p = store_find(s, 42);
    check(p != NULL && p->reading.t == 1001, "equal: find gives the earliest");

    store_walk(s, collect, &c);
    for (i = 0; i < 100 && i < c.n; i++)
        if (c.t[i] != 1001 + i)
            same = 0;
    check(c.n == 100 && same, "equal: walk gives all 100 in arrival order");
    store_destroy(s);
}

/* test_null: a NULL store or record must not crash. */
static void test_null(void)
{
    store_t *s = store_create();
    record_t a = mk(21.2f, 1);
    collector_t c = { .n = 0 };

    check(store_insert(NULL, &a) == -1, "null: insert into NULL store");
    check(store_insert(s, NULL) == -1, "null: insert of NULL record");
    check(store_count(s) == 0, "null: failed inserts change nothing");
    check(store_count(NULL) == 0, "null: count of NULL store");
    check(store_height(NULL) == 0, "null: height of NULL store");
    store_walk(NULL, collect, &c);
    check(c.n == 0, "null: walk of NULL store visits nothing");
    store_walk(s, NULL, NULL);
    store_destroy(s);
    store_destroy(NULL);
    check(1, "null: no crash");
}

int main(void)
{
    test_compare();
    test_empty();
    test_small();
    test_find();
    test_walk_order();
    test_many_distinct();
    test_many_equal();
    test_null();

    printf("%d checks, %d failed\n", checks, failed);
    return failed != 0;
}
