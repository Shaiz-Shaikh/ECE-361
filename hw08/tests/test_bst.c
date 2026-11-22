#include <stdio.h>
#include "../bst.h"

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

/* The original's seven keys: 27 14 35 10 19 31 42. */
static void test_original_keys(void)
{
    int keys[7] = { 27, 14, 35, 10, 19, 31, 42 };
    bst_t *t = bst_create();
    size_t visits = 99;
    int i;

    for (i = 0; i < 7; i++)
        bst_insert(t, keys[i]);
    check(bst_size(t) == 7, "seven keys: size 7");
    check(bst_height(t) == 3, "seven keys: height 3");
    check(bst_contains(t, 31, &visits) && visits == 3,
          "31 is found in 3 visits");
    check(!bst_contains(t, 15, &visits) && visits == 3,
          "15 is not found, in 3 visits");
    check(bst_contains(t, 27, NULL), "visits may be NULL");
    bst_destroy(t);
}

/* An equal key is kept, to the right. */
static void test_duplicate(void)
{
    bst_t *t = bst_create();

    bst_insert(t, 50);
    bst_insert(t, 50);
    check(bst_size(t) == 2, "a duplicate is kept: size 2");
    check(bst_height(t) == 2, "the duplicate goes right: height 2");
    bst_destroy(t);
}

/* Keys in increasing order make a chain: the height is the count. */
static void test_increasing(void)
{
    bst_t *t = bst_create();
    int i;
    int ok = 1;

    for (i = 1; i <= 10; i++) {
        bst_insert(t, i);
        if (bst_height(t) != (size_t)i)
            ok = 0;
    }
    check(ok, "increasing keys: the height equals their number");
    bst_destroy(t);
}

/* A bigger tree, then destroy it. The sanitizer checks the frees. */
static void test_destroy_full(void)
{
    bst_t *t = bst_create();
    int i;
    int ok = 1;

    for (i = 0; i < 100; i++) {
        if (bst_insert(t, (i * 37) % 101) != 0)
            ok = 0;
    }
    check(ok && bst_size(t) == 100, "100 inserts all work: size 100");
    bst_destroy(t);
    check(1, "a full tree was destroyed");
}

/* A NULL tree must not crash anything. */
static void test_null(void)
{
    size_t visits = 99;

    check(bst_insert(NULL, 5) == -1, "insert into NULL fails with -1");
    check(bst_size(NULL) == 0, "size of NULL is 0");
    check(bst_height(NULL) == 0, "height of NULL is 0");
    check(!bst_contains(NULL, 5, &visits) && visits == 0,
          "contains on NULL: false, 0 visits");
    bst_destroy(NULL);
    check(1, "destroying NULL does nothing");
}

/* The input that crashes the original search: a tree with no keys. */
static void test_empty(void)
{
    bst_t *t = bst_create();
    size_t visits = 99;

    check(bst_size(t) == 0, "an empty tree has size 0");
    check(bst_height(t) == 0, "an empty tree has height 0");
    check(!bst_contains(t, 7, &visits) && visits == 0,
          "searching an empty tree: false, 0 visits");
    bst_destroy(t);
}

int main(void)
{
    test_original_keys();
    test_duplicate();
    test_increasing();
    test_destroy_full();
    test_null();
    test_empty();                    /* the one that crashes the port */
    printf("%d checks, %d failed\n", total, failures);
    return failures == 0 ? 0 : 1;
}
