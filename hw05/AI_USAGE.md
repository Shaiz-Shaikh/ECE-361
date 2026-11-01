# AI usage for HW5

**Tool:** Claude (chat), used step by step while I did the homework.

**What I used it for:**
- To explain what the assignment was asking and how a state machine table works.
- Claude helped in writing the C code (lock.h, lock.c, main.c, the tests, the Makefile)
  and explained each part. I typed it into the terminal in small pieces,
  compiled it, ran it, and read the output after every step.

**What I did myself:**
- I filled in the transition table row by row (what happens in each state for
  each event) and traced `3 3 1 4` through it to see why it does not open.
- I ran make, make test and the sample input, and checked that the output
  matched my table.

**Where I was wrong:** in my first try at the table I got two things wrong.
In GOT1 I made "door closes" go back to LOCKED, but the rules say closing
the door while typing a code changes nothing, so it stays in GOT1. In the
OPEN row I put NONE as the next state, mixing up the state and the action.
Claude corrected both and I fixed my table.

**One thing the model got wrong:** Claude's line counts were off. It told me
a file should have 43 lines when it had 42, and it also guessed 80 lines for
the test file, which had 84. The code itself compiled and the tests passed,
so it did not break anything, but I learned not to trust a guessed number
without checking it myself.

**Bonus part:** I used Claude the same way for the table checker. It wrote
check.c and test_check.c and explained the breadth-first search with the
queue, and I typed it in, compiled it, and ran it. I read the checker's
output on the three broken copies and checked it matched what I expected
(for example, that leaving out GOT2 + RIGHT makes OPEN unreachable).
