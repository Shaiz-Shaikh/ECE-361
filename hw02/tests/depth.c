#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../stats.h"

int main(int argc, char *argv[])
{
    char *end;
    long n_long;
    int n;
    float *a;
    float result;

    if (argc != 3) {
        fprintf(stderr, "usage: %s max|tail n\n", argv[0]);
        return 1;
    }

    n_long = strtol(argv[2], &end, 10);
    if (*end != '\0' || n_long < 1 || n_long > 2000000000L) {
        fprintf(stderr, "error: n must be a whole number from 1 to 2000000000\n");
        return 1;
    }
    n = (int) n_long;

    a = malloc((size_t) n * sizeof *a);
    if (a == NULL) {
        fprintf(stderr, "error: could not allocate %d values\n", n);
        return 1;
    }
    for (int i = 0; i < n; i++)
        a[i] = (float) (i % 1000);

    if (strcmp(argv[1], "max") == 0) {
        result = stats_max(a, n);
    } else if (strcmp(argv[1], "tail") == 0) {
        result = stats_max_tail(a, n);
    } else {
        fprintf(stderr, "error: first argument must be max or tail\n");
        free(a);
        return 1;
    }

    printf("%s of %d values: %g\n", argv[1], n, result);
    free(a);
    return 0;
}
