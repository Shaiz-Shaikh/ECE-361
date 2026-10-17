#include <stdio.h>
#include <stddef.h>
#include "../ptr.h"

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

static void test_swap_float(void)
{
    float x = 1.5f;
    float y = 2.5f;
    swap_float(&x, &y);
    check("swap_float exchanges two values", x == 2.5f && y == 1.5f);
}

static void test_minmax(void)
{
    float a[] = {3.0f, -1.0f, 7.5f, 2.0f};
    float one[] = {4.25f};
    float mn = 99.0f;
    float mx = 99.0f;
    int r;

    r = minmax(a, 4, &mn, &mx);
    check("minmax several values", r == 0 && mn == -1.0f && mx == 7.5f);

    r = minmax(one, 1, &mn, &mx);
    check("minmax one value", r == 0 && mn == 4.25f && mx == 4.25f);

    mn = 99.0f;
    mx = 99.0f;
    r = minmax(a, 0, &mn, &mx);
    check("minmax n = 0 returns -1, outputs unchanged",
          r == -1 && mn == 99.0f && mx == 99.0f);

    r = minmax(NULL, 4, &mn, &mx);
    check("minmax NULL array returns -1, outputs unchanged",
          r == -1 && mn == 99.0f && mx == 99.0f);

    r = minmax(a, 4, NULL, &mx);
    check("minmax NULL min returns -1, max unchanged",
          r == -1 && mx == 99.0f);

    r = minmax(a, 4, &mn, NULL);
    check("minmax NULL max returns -1, min unchanged",
          r == -1 && mn == 99.0f);
}

static void test_find_above(void)
{
    float a[] = {1.0f, 5.0f, 3.0f, 7.0f};
    float eq[] = {2.0f, 4.0f, 4.0f};
    float eq2[] = {2.0f, 4.0f, 6.0f};

    check("find_above two matches returns the first",
          find_above(a, 4, 4.0f) == a + 1);
    check("find_above no match returns NULL",
          find_above(a, 4, 10.0f) == NULL);
    check("find_above value equal to threshold is not above",
          find_above(eq, 3, 4.0f) == NULL);
    check("find_above skips equal value and finds the bigger one",
          find_above(eq2, 3, 4.0f) == eq2 + 2);
}

static void test_reverse(void)
{
    float even[] = {1.0f, 2.0f, 3.0f, 4.0f};
    float odd[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    float single[] = {9.0f};

    reverse(even, 4);
    check("reverse even length",
          even[0] == 4.0f && even[1] == 3.0f &&
          even[2] == 2.0f && even[3] == 1.0f);

    reverse(odd, 5);
    check("reverse odd length",
          odd[0] == 5.0f && odd[1] == 4.0f && odd[2] == 3.0f &&
          odd[3] == 2.0f && odd[4] == 1.0f);

    reverse(single, 1);
    check("reverse n = 1 does nothing", single[0] == 9.0f);

    reverse(NULL, 3);
    check("reverse NULL does not crash", 1);
}

int main(void)
{
    test_swap_float();
    test_minmax();
    test_find_above();
    test_reverse();

    printf("\n%d tests run, %d failed\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
