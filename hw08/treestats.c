#include <stdio.h>
#include "bst.h"

/* treestats: reads integers until the end of the input, inserts them
 * in that order, and prints how many keys the tree has and how high
 * it is. */
int main(void)
{
    bst_t *t = bst_create();
    size_t count = 0;               /* values read so far */
    int status = 0;
    int x;
    int r;

    if (t == NULL) {
        fprintf(stderr, "treestats: out of memory\n");
        return 1;
    }

    while ((r = scanf("%d", &x)) == 1) {
        if (bst_insert(t, x) != 0) {
            fprintf(stderr, "treestats: out of memory\n");
            status = 1;
            break;
        }
        count++;
    }

    if (status == 0) {
        if (r == 0) {               /* something that is not a number */
            printf("not a number after %zu values\n", count);
            status = 1;
        } else if (bst_size(t) == 0) {
            printf("empty tree\n");
        } else {
            printf("n %zu, height %zu\n", bst_size(t), bst_height(t));
        }
    }

    bst_destroy(t);                 /* the one place it is freed */
    return status;
}
