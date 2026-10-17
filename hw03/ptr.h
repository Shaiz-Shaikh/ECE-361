#ifndef PTR_H
#define PTR_H

/* swap_float
 * Takes:   a and b, pointers to two floats.
 * Does:    exchanges the values stored at *a and *b.
 * Returns: nothing.
 * Assumes: a and b are not NULL and point to valid floats. */
void swap_float(float *a, float *b);

/* minmax
 * Takes:   a, the first element of an array; n, how many elements;
 *          min and max, where to store the results.
 * Does:    stores the smallest value of a[0..n-1] in *min and the
 *          largest in *max.
 * Returns: 0 on success. Returns -1 and changes nothing if n < 1
 *          or any pointer is NULL.
 * Assumes: a points to at least n floats. */
int minmax(const float *a, int n, float *min, float *max);

/* find_above
 * Takes:   a, the first element of an array; n, how many elements;
 *          threshold, the value to beat.
 * Does:    looks for the first value strictly greater than threshold.
 * Returns: a pointer to that value, or NULL if there is none
 *          (also NULL if a is NULL or n < 1).
 * Assumes: a points to at least n floats. */
const float *find_above(const float *a, int n, float threshold);

/* reverse
 * Takes:   a, the first element of an array; n, how many elements.
 * Does:    reverses a[0..n-1] in place.
 * Returns: nothing. Does nothing if a is NULL or n < 2.
 * Assumes: a points to at least n floats. */
void reverse(float *a, int n);

#endif
