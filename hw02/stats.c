#include "stats.h"

float stats_min(const float a[], int n)
{
    float best = a[0];

    for (int i = 1; i < n; i++) {
        if (a[i] < best)
            best = a[i];
    }
    return best;
}

float stats_max(const float a[], int n)
{
    float rest;

    if (n == 1)
        return a[0];
    rest = stats_max(a + 1, n - 1);
    return a[0] > rest ? a[0] : rest;
}

float stats_mean(const float a[], int n)
{
    float sum = 0.0f;

    for (int i = 0; i < n; i++)
        sum += a[i];
    return sum / n;
}

int stats_longest_run_above(const float a[], int n, float threshold, int *start)
{
    int run = 0, best = 0, best_start = -1, run_start = 0;

    for (int i = 0; i < n; i++) {
        if (a[i] > threshold) {
            if (run == 0)
                run_start = i;
            run++;
            if (run > best) {
                best = run;
                best_start = run_start;
            }
        } else {
            run = 0;
        }
    }
    *start = best_start;
    return best;
}

/* helper for stats_max_tail: best is the largest value seen so far */
static float max_tail_helper(const float a[], int n, float best)
{
    if (n == 0)
        return best;
    if (a[0] > best)
        best = a[0];
    return max_tail_helper(a + 1, n - 1, best);
}

float stats_max_tail(const float a[], int n)
{
    return max_tail_helper(a + 1, n - 1, a[0]);
}
