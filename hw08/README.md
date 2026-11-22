# HW 8: a binary search tree

## Part 2: the crash

I ported the original search as `bst_contains` first, keeping its loop
as it was. Then I wrote `tests/test_bst.c` and ran it with `make memcheck`.
The test that crashes it is `test_empty`: it makes a tree with no keys
and searches it.

First line of the report:

    bst.c:91:21: runtime error: member access within null pointer of type 'struct node'

Why the original never noticed: its main always inserts seven keys before
it searches, so the tree is never empty, and its check for NULL comes one
line after the line that already uses `current`.
