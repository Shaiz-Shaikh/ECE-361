#include <stdio.h>
#include <stdlib.h>
#include "../istack.h"

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

static void test_empty(void)
{
    istack_t *s = istack_create();
    int v = 99;

    check(s != NULL, "create returns a stack");
    check(istack_is_empty(s), "a new stack is empty");
    check(istack_count(s) == 0, "a new stack has count 0");
    check(istack_capacity(s) == ISTACK_START, "a new stack has room for 8");
    check(istack_pop(s, &v) == -1 && v == 99,
          "pop on an empty stack fails and leaves *v alone");
    check(istack_peek(s, &v) == -1 && v == 99,
          "peek on an empty stack fails and leaves *v alone");
    istack_destroy(s);
}

static void test_lifo(void)
{
    istack_t *s = istack_create();
    int v = 0;

    istack_push(s, 1);
    istack_push(s, 2);
    istack_push(s, 3);
    check(istack_peek(s, &v) == 0 && v == 3, "peek shows the last value pushed");
    check(istack_count(s) == 3, "peek does not remove anything");
    check(istack_pop(s, &v) == 0 && v == 3, "pop returns 3 first");
    check(istack_pop(s, &v) == 0 && v == 2, "pop returns 2 next");
    check(istack_pop(s, NULL) == 0, "pop with v NULL works");
    check(istack_is_empty(s), "the stack is empty after three pops");
    istack_destroy(s);
}

static void test_growth(void)
{
    istack_t *s = istack_create();
    int i;
    int v;
    int ok = 1;

    for (i = 0; i < 8; i++)
        istack_push(s, i * 10);
    check(istack_count(s) == 8 && istack_capacity(s) == 8,
          "full at 8: the room is still 8");
    check(istack_push(s, 80) == 0, "the 9th push works");
    check(istack_capacity(s) == 16, "the 9th push doubles the room to 16");
    check(istack_count(s) == 9, "the count is 9 after the 9th push");
    for (i = 8; i >= 0; i--) {
        if (istack_pop(s, &v) != 0 || v != i * 10)
            ok = 0;
    }
    check(ok, "every value is kept across the doubling");
    istack_destroy(s);
}

static void test_many(void)
{
    istack_t *s = istack_create();
    int i;
    int v;
    int ok = 1;

    for (i = 0; i < 1000; i++) {
        if (istack_push(s, i) != 0)
            ok = 0;
    }
    check(ok && istack_count(s) == 1000, "1000 pushes all work");
    check(istack_capacity(s) == 1024, "the room doubled up to 1024");
    for (i = 999; i >= 0; i--) {
        if (istack_pop(s, &v) != 0 || v != i)
            ok = 0;
    }
    check(ok && istack_is_empty(s), "1000 pops come back in reverse order");
    check(istack_capacity(s) == 1024, "the room never shrinks");
    istack_destroy(s);
}

static void test_to_array(void)
{
    istack_t *s = istack_create();
    size_t n = 99;
    int v = 0;
    int *a;

    istack_push(s, 10);
    istack_push(s, 20);
    istack_push(s, 30);
    a = istack_to_array(s, &n);
    check(a != NULL && n == 3, "to_array returns 3 values");
    check(a != NULL && a[0] == 10 && a[1] == 20 && a[2] == 30,
          "to_array is bottom first");
    istack_push(s, 40);
    check(a != NULL && n == 3 && a[2] == 30,
          "a later push does not change the copy");
    if (a != NULL)
        a[0] = 777;
    istack_pop(s, NULL);
    istack_pop(s, NULL);
    istack_pop(s, NULL);
    istack_pop(s, &v);
    check(v == 10, "changing the copy does not change the stack");
    free(a);
    a = istack_to_array(s, &n);
    check(a == NULL && n == 0, "to_array of an empty stack is NULL, n is 0");
    istack_destroy(s);
}

static void test_null(void)
{
    int v = 5;
    size_t n = 7;

    check(istack_push(NULL, 1) == -1, "push on NULL fails");
    check(istack_pop(NULL, &v) == -1 && v == 5,
          "pop on NULL fails and leaves *v alone");
    check(istack_peek(NULL, &v) == -1 && v == 5,
          "peek on NULL fails and leaves *v alone");
    check(istack_count(NULL) == 0, "count of NULL is 0");
    check(istack_is_empty(NULL), "NULL counts as empty");
    check(istack_capacity(NULL) == 0, "capacity of NULL is 0");
    check(istack_to_array(NULL, &n) == NULL && n == 0,
          "to_array of NULL is NULL, n is 0");
    istack_destroy(NULL);
    check(1, "destroy of NULL does nothing");
}

static void test_independent(void)
{
    istack_t *a = istack_create();
    istack_t *b = istack_create();
    int v = 0;
    int i;

    istack_push(a, 1);
    istack_push(a, 2);
    istack_push(b, 100);
    check(istack_count(a) == 2 && istack_count(b) == 1,
          "two stacks keep their own counts");
    for (i = 0; i < 20; i++)
        istack_push(b, i);
    check(istack_capacity(a) == 8 && istack_capacity(b) == 32,
          "growing one stack does not grow the other");
    check(istack_pop(a, &v) == 0 && v == 2, "a still has its own top value");
    istack_destroy(a);
    istack_destroy(b);
}

static void test_destroy_not_empty(void)
{
    istack_t *s = istack_create();
    int i;

    for (i = 0; i < 20; i++)
        istack_push(s, i);
    istack_destroy(s);
    check(1, "destroying a stack that is not empty (memcheck looks for leaks)");
}

int main(void)
{
    test_empty();
    test_lifo();
    test_growth();
    test_many();
    test_to_array();
    test_null();
    test_independent();
    test_destroy_not_empty();
    printf("\n%d tests, %d failed\n", total, failures);
    return failures == 0 ? 0 : 1;
}
