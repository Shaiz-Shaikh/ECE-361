#ifndef STATS_H
#define STATS_H

/*
 * stats_min: smallest value in an array.
 * Takes:   a  the array, n  its length
 * Returns: the smallest of a[0] to a[n-1]
 * Assumes: n >= 1
 */
float stats_min(const float a[], int n);

/*
 * stats_max: largest value in an array, computed recursively.
 * Takes:   a  the array, n  its length
 * Returns: the largest of a[0] to a[n-1]
 * Assumes: n >= 1
 */
float stats_max(const float a[], int n);

/*
 * stats_mean: average of an array.
 * Takes:   a  the array, n  its length
 * Returns: the sum of a[0] to a[n-1] divided by n
 * Assumes: n >= 1
 */
float stats_mean(const float a[], int n);

/*
 * stats_longest_run_above: longest run of consecutive values strictly
 * greater than a threshold. A value equal to the threshold ends a run.
 * Takes:   a  the array, n  its length, threshold  the limit
 *          start  address of an int; set to the index where the longest
 *                 run begins, or -1 if there is no run
 * Returns: the length of the longest run, or 0 if no value is above
 * Assumes: n >= 0, start is not NULL. If two runs tie, the earlier wins.
 */
int stats_longest_run_above(const float a[], int n, float threshold, int *start);

#endif
