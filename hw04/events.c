#include <stdio.h>
#include <string.h>
#include "tally.h"

#define BUF_SIZE 256

int main(void)
{
    char line[BUF_SIZE];
    int skipped = 0;
    tally_t *t = tally_create();

    if (t == NULL) {
        fprintf(stderr, "could not create a tally\n");
        return 1;
    }

    while (fgets(line, sizeof line, stdin) != NULL) {
        int has_newline = (strchr(line, '\n') != NULL);

        line[strcspn(line, "\r\n")] = '\0';

        if (!has_newline && !feof(stdin)) {
            /* the line did not fit in the buffer: throw away the rest */
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            if (line[0] != '#') {
                skipped++;
            }
            continue;
        }

        /* a blank line is empty or has only spaces and tabs */
        if (line[strspn(line, " \t")] == '\0' || line[0] == '#') {
            continue;
        }

        if (tally_add(t, line) == -1) {
            skipped++;
        }
    }

    tally_print(t, stdout);
    printf("names: %d, lines skipped: %d\n", tally_size(t), skipped);
    tally_destroy(t);
    return 0;
}
