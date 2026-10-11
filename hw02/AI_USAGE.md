# AI Usage

I used Claude while working on this homework.

## What I used it for

- Reading the starter program `readings.c` and explaining what each section does.
- Planning how to split it into the modules `reading`, `stats`, and `histogram`.
- Help with writing and explaining the headers, the `.c` files, `main.c`, the Makefile, `tests/test_stats.c`, and the README.
- Learning the Terminal and Git commands I used.

I ran every command myself, read the output, and checked each result with `make test`, which runs the unit tests and the regression test against `expected/`.

## One thing the AI got wrong

When Claude gave me the command to create `README.md`, the module diagram was supposed to be inside a code block. Claude put the triple backticks inside a block that I pasted into Terminal, and the backtick lines were lost. I found it because I ran `cat README.md` after creating the file, and the diagram was missing its code fence, so it would have shown as jumbled text on GitHub. We rewrote the file with the diagram indented by four spaces, which Markdown also treats as a code block, and I checked the file again with `cat`.

## What I verified myself

- `make test` builds from a clean state with no warnings and all tests pass.
- The regression test shows my output is identical to `expected/`.
