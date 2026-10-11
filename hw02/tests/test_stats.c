#include <stdio.h>
#include "../stats.h"

#define BIG 10000

#define CHECK(name, actual, expected)                                   \
    do {                                                                \
        if ((actual) == (expected)) {                                   \
            printf("PASS  %s\n", name);                                 \
            passed++;                                                   \
        } else {                                                        \
            printf("FAIL  %s\n", name);                                 \
            printf("      expected %g, got %g\n",                       \
                   (double)(expected), (double)(actual));               \
            failed++;                                                   \
        }                                                               \
    } while (0)

int main(void)
{
    int passed = 0;
    int failed = 0;
    int start;
    int run;
    int agree;
    float one[1]   = {7.5f};
    float first[3] = {9.0f, 3.0f, 5.0f};
    float last[3]  = {3.0f, 5.0f, 9.0f};
    float runs[4]  = {31.0f, 32.0f, 30.0f, 31.0f};
    float flat[2]  = {30.0f, 30.0f};
    float four[4]  = {1.0f, 2.0f, 3.0f, 4.0f};
    float big[BIG];

    /* 1. one value */
    CHECK("one value: min", stats_min(one, 1), 7.5f);
    CHECK("one value: max", stats_max(one, 1), 7.5f);
    CHECK("one value: mean", stats_mean(one, 1), 7.5f);
    run = stats_longest_run_above(one, 1, 5.0f, &start);
    CHECK("one value: run length above 5", run, 1);
    CHECK("one value: run start", start, 0);

    /* 2. maximum at the start and at the end */
    CHECK("max at the start", stats_max(first, 3), 9.0f);
    CHECK("max at the end", stats_max(last, 3), 9.0f);

    /* 3. a value equal to the threshold ends a run */
    run = stats_longest_run_above(runs, 4, 30.0f, &start);
    CHECK("threshold ends a run: length", run, 2);
    CHECK("threshold ends a run: start", start, 0);
    run = stats_longest_run_above(flat, 2, 30.0f, &start);
    CHECK("all equal to threshold: length", run, 0);
    CHECK("all equal to threshold: start", start, -1);

    /* 4. recursive maximum on 10,000 values */
    for (int i = 0; i < BIG; i++)
        big[i] = (float) i;
    CHECK("10000 values: max at the end", stats_max(big, BIG), 9999.0f);
    big[4000] = 50000.0f;
    CHECK("10000 values: max in the middle", stats_max(big, BIG), 50000.0f);

    /* extra: min and mean */
    CHECK("min of 1 2 3 4", stats_min(four, 4), 1.0f);
    CHECK("mean of 1 2 3 4", stats_mean(four, 4), 2.5f);

    /* bonus: stats_max_tail gives the same answer as stats_max */
    CHECK("tail: one value", stats_max_tail(one, 1), 7.5f);
    CHECK("tail: max at the start", stats_max_tail(first, 3), 9.0f);
    CHECK("tail: max at the end", stats_max_tail(last, 3), 9.0f);
    CHECK("tail: 10000 values, max in the middle",
          stats_max_tail(big, BIG), 50000.0f);
    agree = 1;
    for (int n = 1; n <= BIG; n++) {
        if (stats_max_tail(big, n) != stats_max(big, n))
            agree = 0;
    }
    CHECK("tail agrees with stats_max for every length 1 to 10000", agree, 1);

    printf("\nSummary: %d passed, %d failed\n", passed, failed);
    return failed != 0 ? 1 : 0;
}
