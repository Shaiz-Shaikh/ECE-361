# HW4: Tally (abstract data type)

This folder has my tally. A tally counts how often each name occurs, for
example the events my device sees. The names are kept sorted. Other code can
only use the tally through tally.h. Only tally.c knows what is inside.

## Files

- tally.h: the interface, with a comment for each function
- tally.c: the code. It has the struct and a pool of TALLY_MAX tallies, so
  there is no malloc
- events.c: reads lines from standard input, adds each one to a tally, and
  prints the result
- data/door.txt: an example input file
- tests/test_tally.c: the tests
- Makefile: builds the program and runs the tests
- AI_USAGE.md: how I used AI tools for this homework

## How to build

Go into the hw04 folder and run:

    make

This builds the program events with gcc -std=c11 -Wall -Wextra. It should
print no warnings.

## How to run

    ./events < data/door.txt

For my example file it prints:

    ALARM 2
    DOOR_CLOSE 2
    DOOR_OPEN 3
    names: 3, lines skipped: 1

## How to test

    make test

This builds and runs the tests. Each test prints PASS or FAIL, and the last
line is a summary. The program exits with a nonzero code if any test fails.

## How to clean

    make clean

This removes events, the .o files and the test program.

## Part 3: What the opaque type prevents

I made a small file that includes only tally.h and tried to read a member of
the struct:

    printf("%d\n", t->n);

The compiler gave this error:

    opaque_try.c:7:21: error: incomplete definition of type 'tally_t'
          (aka 'struct tally')
        7 |     printf("%d\n", t->n);
          |                    ~^
    ./tally.h:10:16: note: forward declaration of 'struct tally'
       10 | typedef struct tally tally_t;            /* opaque */
          |                ^
    1 error generated.

The opaque type means that code outside tally.c can only hold a pointer to a
tally and call the functions in tally.h. It cannot read or change anything
inside, like the number of names, the counts or the list of entries. The
compiler stops it, because tally.h says that struct tally exists but never
says what is in it. This is useful for two reasons. First, nobody can break
the rules of the tally by accident, for example by changing a count by hand or
putting the names out of order, because only tally.c touches them. Second, I
can change the inside of tally.c later, for example to use a different way to
store the names, and events.c and the tests will still work without any change,
because they only use the functions.
