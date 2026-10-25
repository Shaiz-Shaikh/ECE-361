# AI usage for HW4

## Tool

I used Claude in a chat.

## What I used it for

Claude wrote the first version of the code (tally.h, tally.c, events.c, the
tests and the Makefile). I typed it into my files step by step, built it, and
ran make test myself. I asked Claude to explain the code line by line, for
example why tally_add moves the later entries up starting from the last one,
and what the opaque type means. Claude also helped me draft the README and
this file. I ran the throwaway file for Part 3 (Claude wrote it) and copied the compiler's
error message into the README from my own terminal.

## One thing the model got wrong

Claude's first events.c only skipped lines that were completely empty. A line
with only a space or a tab was added to the tally as a name. The assignment
says blank lines should be skipped, so this was wrong. I found it when I tried
some odd inputs on purpose: a line with only a space printed a count for the
name " ". We changed the check in events.c so that a line with only spaces and
tabs counts as blank. After the fix, lines with only a space or only a tab
are skipped, and a name with a space in the middle, like DOOR OPEN, still
works.
