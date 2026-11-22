# HW 8: a binary search tree

## Build and test

    make            builds ./treestats
    make test       builds and runs the unit tests (tests/test_bst.c)
    make memcheck   runs the tests again with the address and
                    undefined-behavior sanitizers
    make leaks      runs the tests under the macOS leaks tool
    make clean      removes the build files

`./treestats < file` reads integers until the end of the input and
prints the number of keys and the height of the tree.

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

## Part 3: what order does to a tree

I ran `treestats` on the numbers 1 to 1023, first in increasing order
and then shuffled.

    $ seq 1 1023 | ./treestats
    n 1023, height 1023
    $ seq 1 1023 | sort -R | ./treestats
    n 1023, height 23

Inserting the keys in increasing order makes a chain, because each new
key goes to the right of the last one, so the height is the number of
keys; shuffled order spreads the keys to both sides, so the height is
much smaller. A search follows one path from the root, so the most
visits it can take is the height: up to 1023 for the chain, but only
up to 23 for the shuffled tree.
