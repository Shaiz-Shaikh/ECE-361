#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "../tally.h"

static int tests_run = 0;
static int tests_failed = 0;

static void check(const char *name, int condition)
{
    tests_run++;
    if (condition) {
        printf("PASS: %s\n", name);
    } else {
        printf("FAIL: %s\n", name);
        tests_failed++;
    }
}

static void test_counting_up(void)
{
    tally_t *t = tally_create();

    check("create returns a tally", t != NULL);
    check("a new tally has size 0", tally_size(t) == 0);
    check("first add returns 1", tally_add(t, "DOOR") == 1);
    check("second add returns 2", tally_add(t, "DOOR") == 2);
    check("third add returns 3", tally_add(t, "DOOR") == 3);
    check("count is 3", tally_count(t, "DOOR") == 3);
    check("same name three times is size 1", tally_size(t) == 1);
    tally_destroy(t);
}

static void test_never_added(void)
{
    tally_t *t = tally_create();

    tally_add(t, "DOOR");
    check("a name never added has count 0", tally_count(t, "WINDOW") == 0);
    check("asking for it does not add it", tally_size(t) == 1);
    tally_destroy(t);
}

static void test_empty_name(void)
{
    tally_t *t = tally_create();

    check("an empty name is refused", tally_add(t, "") == -1);
    check("an empty name changes nothing", tally_size(t) == 0);
    tally_destroy(t);
}

static void test_name_lengths(void)
{
    char exact[TALLY_LEN + 1];
    char too_long[TALLY_LEN + 2];
    tally_t *t = tally_create();

    memset(exact, 'A', TALLY_LEN);
    exact[TALLY_LEN] = '\0';
    memset(too_long, 'A', TALLY_LEN + 1);
    too_long[TALLY_LEN + 1] = '\0';

    check("a name of exactly TALLY_LEN characters is accepted",
          tally_add(t, exact) == 1);
    check("it can be counted", tally_count(t, exact) == 1);
    check("a name of TALLY_LEN + 1 characters is refused",
          tally_add(t, too_long) == -1);
    check("the long name was not stored", tally_count(t, too_long) == 0);
    check("only the short name is in the tally", tally_size(t) == 1);
    tally_destroy(t);
}

static void test_null_arguments(void)
{
    tally_t *t = tally_create();

    check("add with a NULL tally is refused", tally_add(NULL, "X") == -1);
    check("add with a NULL name is refused", tally_add(t, NULL) == -1);
    check("count with a NULL tally is 0", tally_count(NULL, "X") == 0);
    check("count with a NULL name is 0", tally_count(t, NULL) == 0);
    check("size of a NULL tally is 0", tally_size(NULL) == 0);
    tally_destroy(NULL);
    check("destroy of NULL does nothing", 1);
    tally_print(NULL, stdout);
    tally_print(t, NULL);
    check("print with NULL arguments does nothing", 1);
    check("NULL arguments changed nothing", tally_size(t) == 0);
    tally_destroy(t);
}

static void test_full_tally(void)
{
    tally_t *t = tally_create();
    char buf[16];
    int i;
    int all_ok = 1;

    for (i = 0; i < TALLY_NAMES; i++) {
        snprintf(buf, sizeof buf, "N%02d", i);
        if (tally_add(t, buf) != 1) {
            all_ok = 0;
        }
    }
    check("TALLY_NAMES different names fit", all_ok);
    check("size is TALLY_NAMES", tally_size(t) == TALLY_NAMES);
    check("a new name is refused when full", tally_add(t, "EXTRA") == -1);
    check("the refused name was not stored", tally_count(t, "EXTRA") == 0);
    check("size is unchanged", tally_size(t) == TALLY_NAMES);
    check("an existing name still counts when full", tally_add(t, "N05") == 2);
    check("its count went up", tally_count(t, "N05") == 2);
    tally_destroy(t);
}

static void test_pool_limit(void)
{
    tally_t *ts[TALLY_MAX];
    int i;
    int all_ok = 1;

    for (i = 0; i < TALLY_MAX; i++) {
        ts[i] = tally_create();
        if (ts[i] == NULL) {
            all_ok = 0;
        }
    }
    check("TALLY_MAX tallies can be created", all_ok);
    check("one more create returns NULL", tally_create() == NULL);
    for (i = 0; i < TALLY_MAX; i++) {
        tally_destroy(ts[i]);
    }
}

static void test_create_after_destroy(void)
{
    tally_t *ts[TALLY_MAX];
    tally_t *again;
    int i;

    for (i = 0; i < TALLY_MAX; i++) {
        ts[i] = tally_create();
    }
    tally_add(ts[0], "OLD");
    tally_destroy(ts[0]);
    again = tally_create();
    check("create works again after a destroy", again != NULL);
    check("the recycled tally is empty", tally_size(again) == 0);
    check("the old name is gone", tally_count(again, "OLD") == 0);
    check("the pool is full again", tally_create() == NULL);
    tally_destroy(again);
    for (i = 1; i < TALLY_MAX; i++) {
        tally_destroy(ts[i]);
    }
}

static void test_independent(void)
{
    tally_t *a = tally_create();
    tally_t *b = tally_create();

    tally_add(a, "X");
    tally_add(a, "X");
    check("tally a counted X twice", tally_count(a, "X") == 2);
    check("tally b has not seen X", tally_count(b, "X") == 0);
    check("tally b is still empty", tally_size(b) == 0);
    tally_add(b, "Y");
    check("tally a has not seen Y", tally_count(a, "Y") == 0);
    check("tally a still has size 1", tally_size(a) == 1);
    tally_destroy(a);
    tally_destroy(b);
}

static void test_print_order(void)
{
    tally_t *t = tally_create();
    FILE *f = tmpfile();
    char out[256];
    size_t n;

    tally_add(t, "pear");
    tally_add(t, "apple");
    tally_add(t, "fig");
    tally_add(t, "banana");
    tally_add(t, "apple");

    check("a temporary file could be opened", f != NULL);
    if (f != NULL) {
        tally_print(t, f);
        rewind(f);
        n = fread(out, 1, sizeof out - 1, f);
        out[n] = '\0';
        check("print is alphabetical, one 'name count' line each",
              strcmp(out, "apple 2\nbanana 1\nfig 1\npear 1\n") == 0);
        fclose(f);
    }
    tally_destroy(t);
}

int main(void)
{
    test_counting_up();
    test_never_added();
    test_empty_name();
    test_name_lengths();
    test_null_arguments();
    test_full_tally();
    test_pool_limit();
    test_create_after_destroy();
    test_independent();
    test_print_order();

    printf("\n%d tests run, %d failed\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
