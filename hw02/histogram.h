#ifndef HISTOGRAM_H
#define HISTOGRAM_H

/*
 * histogram_print: prints a histogram of temperatures to standard output.
 *
 * Prints the line "histogram:" and then one row per non-empty 5 C bin from
 * 0 to 100 C. Values below 0 count in the first bin, and values at or above
 * 100 count in the last bin.
 * Takes:   temps  the temperatures, n  how many there are
 * Returns: nothing
 * Assumes: n >= 0
 */
void histogram_print(const float temps[], int n);

#endif
