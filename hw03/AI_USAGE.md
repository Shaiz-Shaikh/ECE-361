# AI usage for HW3

## Tool

I used Claude in a chat.

## What I used it for

Claude wrote the first version of the code (ptr.h, ptr.c, the tests and the
Makefile). I typed it into my files step by step, built it, and ran make test
myself. After that I asked Claude to explain the code line by line until I got
how the pointers work, like why swap_float needs the * and why minmax only
writes to *min and *max at the end. Claude also helped me draft the README and
this file.

## One thing the model got wrong

Claude told me to use cat -A to check the Tabs in my Makefile. That does not
work on a Mac. I got "illegal option -- A". On macOS you use cat -et instead,
and then the Tabs show up as ^I.
