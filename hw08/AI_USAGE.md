# AI usage for HW8

**Tool:** Claude (chat), used step by step while I did the homework.

**What I used it for:**
- To explain what the homework was asking: what a binary search tree is,
  how a key finds its place, how the original BinarySearchTree.c works,
  and why its search crashes on an empty tree.
- Claude helped in writing the C code (bst.h, bst.c, the tests, the
  Makefile, treestats.c) and explained each part. I typed it into the
  terminal in small pieces, compiled it, ran it, and read the output after
  every step.
- The README was drafted with Claude's help.

**What I did myself:**
- I put in the port of search first, with its loop kept as it was, and ran
  the tests until test_empty crashed it. I ran make memcheck, copied the
  first line of the report into the README, and committed that before I
  fixed bst_contains. The fix is a separate commit.
- After the fix I built with no warnings, ran make test (18 checks, 0
  failed), ran make memcheck (no sanitizer report), and ran leaks on
  test_bst (0 leaks).
- I ran treestats on the three cases from the handout and checked the
  output and the exit status of each. I ran it on 1 to 1023 in increasing
  and in shuffled order and copied both outputs into the README.
- I drew trees by hand (for example where 12 and 33 go) until I could follow
  how insert and search walk down the tree.

**One thing the model got wrong:** Claude told me wc -l would print 44 for
bst.h, but it printed 40. I counted the lines in the file myself and 40 was
right, so the file was fine and the number Claude gave me was wrong.
