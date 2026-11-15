# AI usage for HW7

**Tool:** Claude (chat), used step by step while I did the homework.

**What I used it for:**
- To explain what the homework was asking: how a stack that grows works,
  what malloc, realloc and free do, why realloc goes into a second pointer,
  and how to read a sanitizer report.
- Claude helped in writing the C code (istack, balance, the tests, the
  Makefile and regress.sh) and explained each part. I typed it into the
  terminal in small pieces, compiled it, ran it, and read the output after
  every step.
- The README text was drafted with Claude's help.

**What I did myself:**
- I built with no warnings, ran make test, ran make memcheck (no sanitizer
  report), and ran leaks on both test_istack and balance (0 leaks).
- I made the three bugs from Part 4 on purpose, one at a time, copied the
  first line of each report into the README, then undid each one and ran the
  checks again until they were clean.
- I checked that the regression test can really fail: I added a wrong line to
  expected/deep.txt, saw regress.sh print FAIL and exit with 1, then put the
  file back.
- The six expected files were written from the rules (pairs, deepest, line and
  column), not copied from what the program printed, so the regression test
  compares the program against them.

**One thing the model got wrong:** when leaks reported 80 leaked bytes for the
missing istack_destroy, Claude said it was "a bit more than 24 + 32" because
malloc rounds sizes up. But 24 + 32 is 56, not close to 80, and I did not
check how malloc rounds on my Mac, so that explanation was only a guess. I did
not use it in the README, which only shows the line from leaks and says that
both blocks came from istack_create.
