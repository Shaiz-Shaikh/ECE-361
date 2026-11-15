#include <stdio.h>
#include "istack.h"

/* Returns the closing bracket that goes with an opening one. */
static int closer_of(int open)
{
    if (open == '(')
        return ')';
    if (open == '[')
        return ']';
    return '}';
}

int main(void)
{
    istack_t *s = istack_create();
    int c;
    int top = 0;
    int line = 1;
    int col = 0;
    int pairs = 0;
    int status = 0;
    size_t deepest = 0;

    if (s == NULL) {
        fprintf(stderr, "balance: out of memory\n");
        return 1;
    }

    while ((c = getchar()) != EOF) {
        if (c == '\n') {                    /* a new line starts */
            line++;
            col = 0;
            continue;
        }
        col++;
        if (c == '(' || c == '[' || c == '{') {
            if (istack_push(s, c) != 0) {
                fprintf(stderr, "balance: out of memory\n");
                status = 1;
                break;
            }
            if (istack_count(s) > deepest)
                deepest = istack_count(s);
        } else if (c == ')' || c == ']' || c == '}') {
            if (istack_pop(s, &top) != 0) {
                printf("line %d, column %d: found '%c', nothing open\n",
                       line, col, c);
                status = 1;
                break;
            }
            if (closer_of(top) != c) {
                printf("line %d, column %d: found '%c', expected '%c'\n",
                       line, col, c, closer_of(top));
                status = 1;
                break;
            }
            pairs++;
        }
    }

    if (status == 0) {
        if (istack_is_empty(s)) {
            printf("balanced: %d pairs, deepest %zu\n", pairs, deepest);
        } else {
            istack_peek(s, &top);
            printf("end of input: %zu still open, innermost '%c'\n",
                   istack_count(s), top);
            status = 1;
        }
    }
    istack_destroy(s);                      /* the one place it is freed */
    return status;
}
