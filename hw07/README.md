# Homework 7: istack and balance

## What is here
- `istack.h`, `istack.c`: a stack of int that grows as it fills. It starts with room for 8 values and doubles when it is full, using `realloc`. The struct is defined only in `istack.c`.
- `balance.c`: reads text from standard input and checks that the brackets `( ) [ ] { }` match. It uses the stack.
- `tests/test_istack.c`: the unit tests. Each one prints PASS or FAIL.
- `inputs/`, `expected/`, `regress.sh`: six input files, the expected output and exit status for each, and a script that compares them.
- `Makefile`, `AI_USAGE.md`.

## How to build and test
    make            builds ./balance
    make test       runs the unit tests, then regress.sh
    make memcheck   rebuilds with -fsanitize=address,undefined and runs make test
    make leaks      macOS: runs the unit tests under leaks --atExit
    make clean      removes everything that was built

The sanitizer finds leaks on Linux only. On macOS I look for leaks with:

    leaks --atExit -- ./tests/test_istack
    leaks --atExit -- ./balance < inputs/balanced.txt

To use the checker on a file:

    ./balance < file

It prints one line and exits with 0 if the brackets are balanced and 1 if not.

## Part 4: break it on purpose
For each bug I made the change, ran it, copied the first line of the report, and then undid the change. The code I push has none of these bugs and passes `make memcheck` with no report. Both programs show 0 leaks with `leaks`.

### 1. Destroy the same stack twice
I added a second `istack_destroy(s);` right after the first one in `test_destroy_not_empty`.

    ==31975==ERROR: AddressSanitizer: heap-use-after-free on address 0x603000001240 at pc 0x000104dfebfc bp 0x00016b002f20 sp 0x00016b002f18

What happened: the first `istack_destroy` freed the struct. The second one read `s->data` from that freed struct (`istack.c:30`), so the sanitizer stopped it there, before it could free anything a second time.

### 2. Push onto a stack after destroying it
I added `istack_push(s, 1);` right after the `istack_destroy(s);` in the same test.

    ==32013==ERROR: AddressSanitizer: heap-use-after-free on address 0x603000001248 at pc 0x000102e66d20 bp 0x00016cf9acb0 sp 0x00016cf9aca8

What happened: `istack_push` read `s->count` from the struct that `istack_destroy` had already freed (`istack.c:40`). The pointer `s` still held the old address, but the memory behind it was no longer mine.

### 3. Leave out istack_destroy in balance.c
I deleted the `istack_destroy(s);` line from `balance.c` and ran `leaks --atExit -- ./balance < inputs/balanced.txt`.

    Process 32046: 2 leaks for 80 total leaked bytes.

What happened: the program still printed the right answer, but the two blocks that `istack_create` allocated (the struct and the array) were never freed, so `leaks` found 2 leaks. Both came from `istack_create`.
