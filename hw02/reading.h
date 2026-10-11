#ifndef READING_H
#define READING_H

#include <stdio.h>

/*
 * reading_read: reads sensor readings from the stream `in`.
 *
 * Each good line holds "tick temperature humidity". Blank lines and lines
 * starting with '#' are ignored. A line that does not hold three numbers is
 * counted as skipped.
 *
 * Takes:   in       the stream to read (for example stdin)
 *          ticks, temps, hums
 *                   arrays that this function fills, each with room for
 *                   `max` entries
 *          max      the capacity of the arrays (assumes max >= 1)
 *          skipped  address of an int; set to the number of skipped lines
 * Returns: the number of readings stored (0 to max)
 * Assumes: the arrays are not NULL. If more than `max` readings appear, a
 *          warning goes to stderr and the rest are ignored.
 */
int reading_read(FILE *in, int ticks[], float temps[], float hums[],
                 int max, int *skipped);

#endif
